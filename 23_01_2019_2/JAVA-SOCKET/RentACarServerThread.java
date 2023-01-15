import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;

class RentACarServerThread extends Thread {
    private Socket clientSocket = null;
    private int buffer_size = 4096;
    private static Prenotazione [] prenotazioni;
    private static final int MAX_PREN = 10;

    public RentACarServerThread(Socket clientSocket) {
        this.clientSocket = clientSocket;

        if ( prenotazioni == null ) {
            prenotazioni = new Prenotazione[MAX_PREN];

            for ( int i = 0; i < MAX_PREN; i++ ) {
                prenotazioni[i] = new Prenotazione("L", "L", "L", "L");
            }

            prenotazioni[0] = new Prenotazione("AA111AA", "12345", "auto", "AA111AA_img/");
        }
    }

    public void run() {
        System.out.println("Attivazione figlio: " + Thread.currentThread().getName());

        DataInputStream inSock;
        DataOutputStream outSock;

        
        try {
            inSock = new DataInputStream(clientSocket.getInputStream());
            outSock = new DataOutputStream(clientSocket.getOutputStream());
        } catch (IOException ioe) {
            System.out.println("Problemi nella creazione degli stream di input/output su socket: ");
            ioe.printStackTrace();
            return;
        }
        try {
            try {
                String comandoRicevuto, targa;
                long numeroByte;
                int nread;
                File downloadFolder, mustRemove;
                FileInputStream imageFileStream;
                byte[] buffer = new byte[buffer_size];
                boolean success;

                while ((comandoRicevuto = inSock.readUTF()) != null) {
                    
                    //ELIMINA PRENOTAZIONE
                    if ( comandoRicevuto.equals("E") ) {
                        // Leggo Targa
                        targa = inSock.readUTF();
                        success = false;

                        for ( int i = 0; i < MAX_PREN; i++ ) {
                            if ( prenotazioni[i].getTarga().equals(targa) ) {
                                prenotazioni[i] = new Prenotazione("L", "L", "L", "L");
                                success = true;

                                mustRemove = new File(targa + "_img/");

                                if ( mustRemove.exists() && mustRemove.isDirectory() ) {
                                    mustRemove.delete();
                                }
                            }
                        }

                        outSock.writeBoolean(success);
                        continue;
                    
                    // DOWNLOAD FOTO
                    } else if ( comandoRicevuto.equals("D")) {

                        targa = inSock.readUTF();

                        System.out.println(targa);
                        downloadFolder = new File(targa + "_img/");
                        
                        if ( downloadFolder.exists() && downloadFolder.isDirectory() && downloadFolder.canRead()) {
                            outSock.writeBoolean(true);

                            System.out.println("Folder Presente");

                            // INVIO NUMERO FILE
                            outSock.writeInt(downloadFolder.listFiles().length);
                            for ( File image : downloadFolder.listFiles() ) {

                                // SCRIVI NOME
                                outSock.writeUTF(image.getName());

                                // APRO STREAM DEL FILE
                                imageFileStream = new FileInputStream(image);

                                numeroByte = image.length();

                                // INVIO NUMERO BYTE DEL FILE
                                outSock.writeLong(numeroByte);

                                while ( numeroByte > 0 ) {
                                    nread = imageFileStream.read(buffer);
                                    outSock.write(buffer, 0, nread);

                                    numeroByte -= nread;
                                }

                                imageFileStream.close();
                            }
                        } else {
                            outSock.writeBoolean(false);
                        }
                    }
                } // while
            } catch (EOFException eof) {
                System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
                clientSocket.close();
                System.out.println("RentACarServer: termino...");
                System.exit(0);
            } catch (SocketTimeoutException ste) {
                System.out.println("Timeout scattato: ");
                ste.printStackTrace();
                clientSocket.close();
                System.exit(1);
            } catch (Exception e) {
                System.out.println("Problemi, i seguenti : ");
                e.printStackTrace();
                System.out.println("Chiudo ed esco...");
                clientSocket.close();
                System.exit(2);
            }
        } catch (IOException ioe) {
            System.out.println("Problemi nella chiusura della socket: ");
            ioe.printStackTrace();
            System.out.println("Chiudo ed esco...");
            System.exit(3);
        }
    }

}// thread