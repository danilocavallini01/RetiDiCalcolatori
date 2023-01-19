import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;

class EventiServerThread extends Thread {
    private Socket clientSocket = null;

    private static String [][] eventi = null;
    private static final int N = 10;

    public EventiServerThread(Socket clientSocket) {
        this.clientSocket = clientSocket;

        if ( eventi == null ) {
            eventi = new String[N][6];
            
            for ( int i = 0; i < N; i++ ) {
                for ( int j = 0; j < 6; j++ ) {
                    eventi[i][j] = "L";
                }
            }

            eventi[0][0] = "String";
            eventi[0][1] = "Concerto";
            eventi[0][2] = "11/01/2014";
            eventi[0][3] = "Verona";
            eventi[0][4] = "40";
            eventi[0][5] = "40"; 

            eventi[1][0] = "Junentus-Inger";
            eventi[1][1] = "Calcio";
            eventi[1][2] = "03/05/2014";
            eventi[1][3] = "Torino";
            eventi[1][4] = "21";
            eventi[1][5] = "150";
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
                String command, tipo, luogo;
                int i, prezzo;

                while((command = inSock.readUTF()) != null ) {
                    
                    if ( command.equalsIgnoreCase("V") ) {
                        // VISUALIZZA EVENTI DI UN CERTO TIPO E LUOGO

                        // LEGGO TIPO
                        tipo = inSock.readUTF();

                        // LEGGO LUOGO
                        luogo = inSock.readUTF();

                        // CONTO QUANTI EVENTI SODDISFANO RICHIESTA
                        i = 0;
                        for ( String [] e : eventi ) {
                            if ( e[1].equalsIgnoreCase(tipo) && e[3].equalsIgnoreCase(luogo)) {
                                i++;
                            }
                        }

                        // INVIO NUMERO DI EVENTI TROVATI
                        outSock.writeInt(i);
                        
                        // INVIO EVENTI TROVATI
                        for ( String [] e : eventi ) {
                            if ( e[1].equalsIgnoreCase(tipo) && e[3].equalsIgnoreCase(luogo)) {
                                for ( String param : e ) {
                                    outSock.writeUTF(param);
                                }
                            }
                        }
                    } else if ( command.equalsIgnoreCase("D") ) {
                        // VISUALIZZA EVENTI SOTTO CERTO PREZZO

                        // LEGGO PREZZO
                        prezzo = inSock.readInt();

                        i = 0;

                        for ( String[] e : eventi ) {
                            if ( !e[5].equalsIgnoreCase("L") && Integer.parseInt(e[5]) <= prezzo ) {
                                i++;
                            }
                        }

                        // INVIO NUMERO DI EVENTI TROVATI
                        outSock.writeInt(i);

                        // INVIO EVENTI TROVATI
                        for ( String [] e : eventi ) {
                            if ( !e[5].equalsIgnoreCase("L") && Integer.parseInt(e[5]) <= prezzo ) {
                                for ( String param : e ) {
                                    outSock.writeUTF(param);
                                }
                            }
                        }
                    }
                }
            } catch (EOFException eof) {
                System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
                clientSocket.close();
                System.out.println("ServerStreamThread: termino...");
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
            } finally {
                clientSocket.close();
            }
        } catch (IOException ioe) {
            System.out.println("Problemi nella chiusura della socket: ");
            ioe.printStackTrace();
            System.out.println("Chiudo ed esco...");
            System.exit(3);
        }
    }

}// thread