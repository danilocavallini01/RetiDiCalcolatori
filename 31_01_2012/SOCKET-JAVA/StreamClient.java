import java.net.*;
import java.io.*;

public class StreamClient {

    public static void main(String[] args) throws IOException {
        InetAddress addr = null;
        int port = -1;

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        
        try {
            if (args.length == 2) {
                addr = InetAddress.getByName(args[0]);
                port = Integer.parseInt(args[1]);
                if (port < 1024 || port > 65535) {
                    System.out.println(
                            "Usage: java EventiClient serverHost serverPort");
                    System.exit(1);
                }

            } else {
                System.out.println("Usage: java EventiClient serverHost serverPort");
                System.exit(1);
            }
        } catch (Exception e) {
            System.out.println("Problemi, i seguenti: ");
            e.printStackTrace();
            System.out.println("Usage: java EventiClient serverHost serverPort");
            System.exit(2);
        }

        // SOCKET STREAM VARIABLES
        Socket socket = null;
        DataInputStream inSock = null;
        DataOutputStream outSock = null;

        // SOCKET STREAM
        try {
            socket = new Socket(addr, port);
            socket.setSoTimeout(30000);
            System.out.println("Creata la socket: " + socket);
            inSock = new DataInputStream(socket.getInputStream());
            outSock = new DataOutputStream(socket.getOutputStream());
        } catch (IOException ioe) {
            System.out.println("Problemi nella creazione degli stream su socket: ");
            ioe.printStackTrace();
            System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
            System.exit(1);
        }

        String command, fileName, giorno;
        int nread, buffer_size = 4096, soglia;
        long numFile, fileLength, nByte;
        FileOutputStream fOutputStream;
        byte [] buffer = new byte[buffer_size];
        File file;
        FileInputStream fileStream;

        try {
            System.out.print("Inserire (U=UploadDelAudio,A=AudioTraDueTemp):");
            while (( command =  stdIn.readLine()) != null ) {

                if ( command.equalsIgnoreCase("U") ) {
                    // UPLOAD DEI FILE
                    
                    outSock.writeUTF(command);

                    System.out.println("Inserire giorno della settimana");
                    giorno = stdIn.readLine();

                    System.out.println("Inserire nome del file: ");
                    fileName = stdIn.readLine();

                    file = new File(fileName);
                    
                    if ( !file.exists() || !file.canRead() || !file.isFile() ) {
                        System.out.print("Inserire (U=UploadDelAudio,A=AudioTraDueTemp):");
                        continue;
                    }

                    // SCRIVO GIORNO
                    outSock.writeUTF(giorno);

                    // LEGGO SE HO TROVATO LI GIORNO
                    if ( !inSock.readBoolean() ) {
                        System.out.print("Inserire (U=UploadDelAudio,A=AudioTraDueTemp):");
                        continue;
                    }

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

                }  else if ( command.equalsIgnoreCase("A") ) {
                    // LEGGO FILE BINARI DI UN DIRETTORIO
                    
                    outSock.writeUTF(command);

                    System.out.println("Inserire soglia: ");
                    try {
                        soglia = Integer.parseInt(stdIn.readLine());
                    } catch ( NumberFormatException e ) {
                        System.out.print("Inserire (U=UploadDelAudio,A=AudioTraDueTemp):");
                        continue;
                    }

                    // SCRIVO SOGLIA
                    outSock.writeInt(soglia);

                    // LEGGO GIORNI TROVATI IN TOTALE
                    numFile = inSock.readInt();

                    System.out.println(numFile + " file trovati");
                    for ( int i = 0; i < numFile; i++ ) {
                        // LEGGO NOME FILE
                        fileName = inSock.readUTF();
                        System.out.println(fileName);
    
                        // LEGGO LUNGHEZZA FILE
                        fileLength = inSock.readLong();
        
                        // SCRIVO BYTE SU FILE 
                        fOutputStream = new FileOutputStream(fileName);
    
                        while ( fileLength > 0 ) {
                            nread = inSock.read(buffer, 0, fileLength > buffer_size ? buffer_size : (int)fileLength);
                            fOutputStream.write(buffer, 0, nread);
    
                            fileLength -= nread;
                        }
    
                        fOutputStream.close();
                    }

                }
                System.out.print("Inserire (U=UploadDelAudio,A=AudioTraDueTemp):");
            }
        } catch (IOException ioe) {
            System.out.println("Problemi Di Operazioni Input ");
            ioe.printStackTrace();
            socket.close();
            System.out.println("Chiudo ed esco...");
            System.exit(2);
        }
      
        socket.close();
    }
}
