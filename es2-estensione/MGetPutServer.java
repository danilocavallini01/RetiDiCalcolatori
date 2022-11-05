import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileOutputStream;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class MGetPutServer {
    private static final int port = 54321;

    public static void main(String args[]) {
        ServerSocket serverSocket = null;
        Socket socket = null;
    
        try {
            serverSocket = new ServerSocket(port);
            serverSocket.setReuseAddress(true);
        } catch (Exception e) {
            System.err.println("Problemi nella creazione della server socket: " + e.getMessage());
            e.printStackTrace();
            System.exit(2);
        }

        while (true) {
            System.out.println("PutFileServer: avviato ");
            System.out.println("Creata la server socket: " + serverSocket);

            try {
                socket = serverSocket.accept();
                new PutFileServerThread(socket).start();
            } catch (IOException e) {
                e.printStackTrace();
                continue;
            }  
        }
    }

}

class PutFileServerThread extends Thread {
    private Socket socket;
    private File dir;
    private String file, path;  
    private long dim;
    private int numeroFile;

    private DataInputStream inSock = null;
    private DataOutputStream outSock = null;
    private FileOutputStream outFile = null;

    public PutFileServerThread(Socket socket) {
        this.socket = socket;
    }

    public void run() {
        try {
            inSock = new DataInputStream(socket.getInputStream());
            outSock = new DataOutputStream(socket.getOutputStream());

            FTPHandler.GET(outSock, inSock, socket);

        } catch (EOFException e) {
            System.out.println("\nRicezione eseguita con successo da un host");
            System.out.println(">------------------------------------<");
        } catch (NumberFormatException e) {
            System.err.println("Problemi nella socket: " + e.getMessage());
            e.printStackTrace();
        } catch (IOException e ) {
            System.err.println("Problemi nella socket: " + e.getMessage());
            e.printStackTrace();
        }
    }
}