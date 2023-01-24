import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;

class ServerStreamThread extends Thread {
    private Socket clientSocket = null;
    private static Giorno [] giorni = null;
    private static final int N = 7;

    public ServerStreamThread(Socket clientSocket) {
        this.clientSocket = clientSocket;

        if ( giorni == null ) {
            giorni = new Giorno[N];
    
            for ( int i = 0; i < N; i++ ) {
                giorni[i] = new Giorno("L", "L", -255, -255, "L");
            }

            giorni[0] = new Giorno("Lunedi","nuvoloso", -2, 6, "./audio/cao.txt");
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
                String command;
                long nByte;
                FileInputStream fileStream;

                String giorno, fileName = null;
                int buffer_size = 4096, soglia, foundDays, nread;
                int found;
                long fileLength;
                byte [] buffer = new byte[buffer_size];
                FileOutputStream fOutputStream;
                File file;

                while((command = inSock.readUTF()) != null ) {
                    
                    if ( command.equalsIgnoreCase("U") ) {
                        // UPLOAD DEI FILE 

                        // RICEVO GIORNO DELLA SETTIMANA
                        giorno = inSock.readUTF();

                        found = -1;
                        for ( int i = 0; i < N; i++ ) {
                            if ( giorno.equalsIgnoreCase(giorni[i].getGiorno()) ) {
                                found = i;
                                break;
                            }
                        }
                        
                        // SCRIVO SE HO TROVATO IL GIORNO
                        outSock.writeBoolean(found != -1);

                        if ( found == -1) {
                            continue;
                        }

                        // LEGGO NOME FILE
                        fileName = inSock.readUTF();
                        
                        giorni[found].setAudio(fileName);

                        // LEGGO LUNGHEZZA FILE
                        fileLength = inSock.readLong();
        
                        // SCRIVO BYTE SU FILE 
                        fOutputStream = new FileOutputStream(fileName);
    
                        // SCRIVO FILE
                        while ( fileLength > 0 ) {
                            nread = inSock.read(buffer, 0, fileLength > buffer_size ? buffer_size : (int)fileLength);
                            fOutputStream.write(buffer, 0, nread);
    
                            fileLength -= nread;
                        }

                        fOutputStream.close();
                    } else if ( command.equalsIgnoreCase("A") ) {
                        // AUDIO SOGLIA TRA DUE TEMPERATURE

                        // LEGGO SOGLIA
                        soglia = inSock.readInt();
                        
                        foundDays = 0;
                        for ( int i = 0; i < N; i++ ) {
                            if ( !giorni[i].getGiorno().equals("L") && (giorni[i].gettMax() - giorni[i].gettMin()) > soglia ) {
                                file = new File(giorni[i].getAudio());

                                if ( file.exists() && file.canRead() && file.isFile() ) {
                                    foundDays++;
                                }
                            }
                        }

                        // SCRIVI FILE TROVATI
                        outSock.writeInt(foundDays);


                        for ( int i = 0; i < N; i++ ) {
                            if ( !giorni[i].getGiorno().equals("L") && (giorni[i].gettMax() - giorni[i].gettMin()) > soglia ) {
                                file = new File(giorni[i].getAudio());

                                if ( file.exists() && file.canRead() && file.isFile() ) {
                                    // SCRIVI NOME
                                    outSock.writeUTF(file.getName());

                                    // APRO STREAM DEL FILE
                                    fileStream = new FileInputStream(file);

                                    nByte = file.length();

                                    // INVIO NUMERO BYTE DEL FILE
                                    outSock.writeLong(nByte);

                                    // INVIO NUMERO FILE
                                    while ( nByte > 0 ) {
                                        nread = fileStream.read(buffer);
                                        outSock.write(buffer, 0, nread);

                                        nByte -= nread;
                                    }

                                    fileStream.close();
                                }
                            }
                        }
                    }
                }
            } catch (EOFException eof) {
                System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
                clientSocket.close();
                System.out.println("ServerStreamThread: termino...");
                return;
            } catch (SocketTimeoutException ste) {
                System.out.println("Timeout scattato: ");
                ste.printStackTrace();
                clientSocket.close();
                return;
            } catch (Exception e) {
                System.out.println("Problemi, i seguenti : ");
                e.printStackTrace();
                System.out.println("Chiudo ed esco...");
                clientSocket.close();
                return;
            } finally {
                clientSocket.close();
            }
        } catch (IOException ioe) {
            System.out.println("Problemi nella chiusura della socket: ");
            ioe.printStackTrace();
            System.out.println("Chiudo ed esco...");
            return;
        }
    }

}// thread