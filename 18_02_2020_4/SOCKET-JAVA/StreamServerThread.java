import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;

class StreamServerThread extends Thread {
    private Socket clientSocket = null;

    private static Song [] songs = null;
    private static final int N = 10;

    public StreamServerThread(Socket clientSocket) {
        this.clientSocket = clientSocket;

        if ( songs == null ) {
            songs = new Song[N];
            
            for ( int i = 0; i < N; i++ ) {
                songs[i] = new Song("L", "L" , -1 , "L");
            }

            songs[0] = new Song("Brasco","Io odio il giovedì",100,"odioGiov.txt");
            songs[1] = new Song("Viga", "Bar da alfredo", 50, "alfredo.txt");
            songs[2] = new Song("Maneskin", "Bruh", 20, "mane.txt");
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
                String command, title, foundFileName;
                boolean titleFound;
                File file;
                FileReader fr;
                long nByte;
                int chr;
                int maxVote1, maxVote2;
                String maxVSinger1, maxVSinger2;

                while((command = inSock.readUTF()) != null ) {
                    
                    if ( command.equalsIgnoreCase("R") ) {
                        // REPERIMENTO DI FILE DA TITOLO

                        // LEGGO TITOLO CANZONE
                        title = inSock.readUTF();

                        titleFound = false;
                        foundFileName = null;
                        
                        for ( int i = 0; i < N; i++ ) {
                            if ( !songs[i].getTitle().equals("L") && songs[i].getTitle().equalsIgnoreCase(title) ) {
                                foundFileName = songs[i].getFileName();
                                titleFound = true;
                                break;
                            }
                        }

                        // SCRIVO SE TITOLO ESISTE 
                        outSock.writeBoolean(titleFound);
                        
                        if ( !titleFound ) {
                            continue;
                        }

                        file = new File(foundFileName);

                        if ( !file.exists() || !file.isFile() || !file.canRead() ) {
                            // SCRIVO SE FILE ESISTE
                            outSock.writeBoolean(false);
                            continue;
                        }

                        // SCRIVO SE FILE ESISTE
                        outSock.writeBoolean(true);

                        // INVIO FILE

                        // SCRIVI NOME
                        outSock.writeUTF(file.getName());

                        // APRO STREAM DEL FILE
                        fr = new FileReader(file);

                        nByte = file.length();

                        // INVIO NUMERO BYTE DEL FILE
                        outSock.writeLong(nByte);

                        // INVIO NUMERO FILE
                        while ( (chr = fr.read()) > 0 && nByte > 0 ) {
                            outSock.writeChar(chr);
                            nByte--;
                        }

                        fr.close();
                    } else if ( command.equalsIgnoreCase("V") ) {
                        // VISUALIZZA LE DUE CANZONI PIU' VOTATE

                        maxVote1 = -1;
                        maxVote2 = -1;
                        maxVSinger1 = null;
                        maxVSinger2 = null;

                        for ( int i = 0; i < N; i++ ) {
                            if ( !songs[i].getSinger().equals("L") ) {
                                if ( songs[i].getVotes() > maxVote1 ) {
                                    maxVote1 = songs[i].getVotes();
                                    maxVSinger1 = songs[i].getSinger();
                                } else if ( songs[i].getVotes() > maxVote2 ) {
                                    maxVote2 = songs[i].getVotes();
                                    maxVSinger2 = songs[i].getSinger();
                                }
                            }
                        }

                        if ( maxVote1 == -1 && maxVote2 == -1 ) {
                            // SCRIVO QUANTE CANZONI TROVATE
                            outSock.writeInt(0);
                        } else if (maxVote2 == -1 ) {
                            // SCRIVO QUANTE CANZONI TROVATE
                            outSock.writeInt(1);

                            // SCRIVO CANTANTI/E
                            outSock.writeUTF(maxVSinger1);
                        } else {
                            // SCRIVO QUANTE CANZONI TROVATE
                            outSock.writeInt(2);

                            // SCRIVO CANTANTI/E
                            outSock.writeUTF(maxVSinger1);
                            // SCRIVO CANTANTI/E
                            outSock.writeUTF(maxVSinger2);
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