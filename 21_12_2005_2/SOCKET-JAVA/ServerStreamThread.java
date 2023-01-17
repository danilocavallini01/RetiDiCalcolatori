import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;

class ServerStreamThread extends Thread {
    private Socket clientSocket = null;

    private static Stanza [] stanze = null;
    private static final int N = 10;
    private static final int K = 8;

    public ServerStreamThread(Socket clientSocket) {
        this.clientSocket = clientSocket;

        if ( stanze == null ) {
            stanze = new Stanza[N];
            String [] utenti;

            for ( int i = 0; i < N; i++ ) {
                utenti = new String[K];
            
                for ( int j = 0; j < K; j++ ) {
                    utenti[j] = "L";
                }

                stanze[i] = new Stanza("L", "L", utenti);
            }

            utenti = new String[K];
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
                String row;
                String [] utenti;

                // SCRIVO NUMERO DI RIGHE
                outSock.writeInt(N);

                for ( int i = 0; i < N; i++ ) {
                    row = stanze[i].getNome() + "|" + stanze[i].getStato();

                    utenti = stanze[i].getUtenti();

                    for ( int j = 0; j < K; j++ ) {
                        row += "|" + utenti[j];
                    }

                    row += "\n";

                    // SCRIVO RIGA
                    outSock.writeUTF(row);
                }
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