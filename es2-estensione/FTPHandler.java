import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.Socket;

public class FTPHandler {

    public static void PUT(DataOutputStream outSock, DataInputStream inSock, File[] directories, Long dimMin) {
        FileInputStream inFile;
        File[] files;

        Long lunghezza;
        int numeroFile;

        String nomeFile, path;
        String absolute;
        String comando;

        try {
            for (File dir : directories) {
                numeroFile = 0;
                path = dir.getName();
                files = dir.listFiles();

                for (File file : files) {
                    if (file.length() > dimMin && !file.isDirectory()) {
                        numeroFile++;
                    }
                }

                System.out.println(">---------------------------------------<");
                outSock.writeUTF(path);
                System.out.println(">> directory: " + path);

                outSock.writeInt(numeroFile);
                System.out.println(">> numero file: " + numeroFile);

                for (File file : files) {
                    nomeFile = file.getName();
                    absolute = file.getAbsolutePath();
                    lunghezza = file.length();

                    if (lunghezza < dimMin || file.isDirectory()) {
                        continue;
                    }

                    try {
                        outSock.writeUTF(nomeFile);
                        System.out.println("\n>> nome file : " + nomeFile);
                        System.out.println("|  inizio la trasmissione di " + nomeFile);

                        comando = inSock.readUTF();
                        System.out.println("<< comando: " + comando);

                        if (comando.equals("attiva")) {
                            inFile = new FileInputStream(absolute);
                            System.out.println(">> dimensione del file: " + file.length());
                            outSock.writeLong(file.length());
                            FileUtility.trasferisci_a_byte_file_binario(new DataInputStream(inFile), outSock,
                                    file.length());
                            inFile.close();
                        }

                    } catch (Exception e) {

                        System.out.println("Problemi nell'invio del nome di " + nomeFile + ": ");
                        e.printStackTrace();
                        continue;
                    }
                }

            }
        } catch (Exception e) {

            System.err.println("Errore irreversibile, il seguente: ");
            e.printStackTrace();
            System.err.println("Chiudo!");
            System.exit(3);
        }
    }

    public static void GET(DataOutputStream outSock, DataInputStream inSock, Socket socket) 
        throws EOFException, NumberFormatException, IOException {

        File dir;
        String file, path;  
        long dim;
        int numeroFile;

        FileOutputStream outFile = null;
        
        while (!socket.isClosed() ) {
            path = inSock.readUTF();
            dir = new File(path);

            if ( !dir.exists() ) {
                dir.mkdir();
            }

            System.out.println(">------------------------------------<");
            System.out.println("<< directory: " + path);

            numeroFile = inSock.readInt();
            System.out.println("<< numero files: " + numeroFile);

            for ( int i = 0; i < numeroFile; i++) {
                file = path + File.separator + inSock.readUTF();
                System.out.println("\n<< file: " + file);

                if (new File(file).exists()) {
                    outSock.writeUTF("salta");
                    System.out.println("|| -- SALTA  --");
                } else {
                    outSock.writeUTF("attiva");

                    dim = inSock.readLong();

                    System.out.println("|| -- ATTIVA  --");
                    System.out.println("<< file di dimensione: " + dim);

                    outFile = new FileOutputStream(file);
                    FileUtility.trasferisci_a_byte_file_binario(inSock,new DataOutputStream(outFile), dim);
                    outFile.close();
                    System.out.println("$$ finito di trasferire file: " + file);
                }
            }
            
        }
    }
}
