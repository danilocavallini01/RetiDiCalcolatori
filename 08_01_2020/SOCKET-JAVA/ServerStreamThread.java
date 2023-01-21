import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;

class ServerStreamThread extends Thread {
    private Socket clientSocket = null;

    public ServerStreamThread(Socket clientSocket) {
        this.clientSocket = clientSocket;
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
                String command, dir_name;
                char chr;
                int occorrenze, res, cread, occ_in_linea, n_file, nread;
                long nByte;
                boolean found_in_line;
                File dir;
                FileReader fr;
                FileInputStream fileStream;
                byte [] buffer = new byte[4096];

                while((command = inSock.readUTF()) != null ) {
                    
                    if ( command.equalsIgnoreCase("C") ) {
                        // CONTEGGIO NUMERO LINEE CON OCCORRENZE

                        // LEGGO CARATTERE
                        chr = inSock.readChar();

                        // LEGGO OCCORRENZE
                        occorrenze = inSock.readInt();
                        // CHIUDO INPUT
                        //clientSocket.shutdownInput();

                        // LEGGO DIR CORRENTE
                        dir = new File(".");
                        res = 0;
                        found_in_line = false;

                        for ( File f : dir.listFiles() ) {
                            if ( f.isFile() && f.canRead() && f.getName().endsWith(".txt") ) {
                                fr = new FileReader(f);

                                occ_in_linea = 0;
                                while ((cread = fr.read()) != -1) {
                                    if ( cread == '\n' ) {
                                        occ_in_linea = 0;
                                        found_in_line = false;

                                    } else if ( cread == chr && !found_in_line ) {
                                        occ_in_linea++;

                                        if ( occ_in_linea == occorrenze ) {
                                            res++;
                                            found_in_line = true;
                                        }
                                    }
                                }

                                fr.close();
                            }
                        }

                        // SCRIVO CARATTERI TROVATI
                        outSock.writeInt(res);
                    } else if ( command.equalsIgnoreCase("B") ) {
                        // LEGGO FILE BINARI DI UN DIRETTORIO

                        // LEGGO DIRETTORIO
                        dir_name = inSock.readUTF();

                        dir = new File(dir_name);

                        if ( !dir.exists() || !dir.canRead() || !dir.isDirectory() ) {
                            // SCRIVO SE DIR ESISTE E VALIDA
                            outSock.writeBoolean(false);
                            continue;
                        }

                        // SCRIVO SE DIR ESISTE E VALIDA
                        outSock.writeBoolean(true);

                        // CONTO QUANTI FILE CI SONO
                        n_file = 0;
                        for ( File f : dir.listFiles() ) {
                            if ( f.canRead() && f.isFile() && !f.getName().endsWith(".txt") ) {
                                n_file++;
                            }
                        }
                        // INVIO NUMERO FILE
                        outSock.writeInt(n_file);

                        for ( File f : dir.listFiles() ) {
                            if ( f.canRead() && f.isFile() && !f.getName().endsWith(".txt") ) {
                                
                                // SCRIVI NOME
                                outSock.writeUTF(f.getName());

                                // APRO STREAM DEL FILE
                                fileStream = new FileInputStream(f);

                                nByte = f.length();

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