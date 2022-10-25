import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileOutputStream;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class PutFileServer {
    private static final int port = 54321;

    public static void main(String args[]) {
        ServerSocket serverSocket = null;
        Socket socket = null;
        String file = null;
        
        long dim;

        DataInputStream inSock = null;
        DataOutputStream outSock = null;
        FileOutputStream outFile = null;
    
        try {
            serverSocket = new ServerSocket(port);
            serverSocket.setReuseAddress(true);
        } catch (Exception e) {
            System.err.println("Problemi nella creazione della server socket: " + e.getMessage());
            e.printStackTrace();
            System.exit(2);
        }

        while (true) {
            System.out.println("PutFileServerSeq: avviato ");
            System.out.println("Creata la server socket: " + serverSocket);

            try {
                socket = serverSocket.accept();

                inSock = new DataInputStream(socket.getInputStream());
                outSock = new DataOutputStream(socket.getOutputStream());

                while ((file = inSock.readUTF()) != null) {
                    System.out.println(">------------------------------------");
                   
                    System.out.println("Ricevuto dal client file: " + file);

                    if (new File(file).exists()) {
                        outSock.writeUTF("salta");
                        System.out.println("File gia presente: salta");
                    } else {
                        outSock.writeUTF("attiva");

                        dim = inSock.readLong();

                        System.out.println("File non presente: attiva");
                        System.out.println("File di dimensione: " + dim);

                        outFile = new FileOutputStream(file);
                        FileUtility.trasferisci_a_byte_file_binario(inSock,new DataOutputStream(outFile), dim);
                        outFile.close();
                        System.out.println("Finito di trasferire file: " + file);
                    }
                }
                

                socket.close();
                
            } catch (EOFException e) {
                System.out.println("Ricezione eseguita con successo da un host");
                System.out.println("##########################################");
                continue;
            } catch (NumberFormatException e) {
                System.err.println("Problemi nella socket: " + e.getMessage());
                e.printStackTrace();
                continue;
            } catch (IOException e ) {
                System.err.println("Problemi nella socket: " + e.getMessage());
                e.printStackTrace();
                continue;
            }
            
        }

    }

}
