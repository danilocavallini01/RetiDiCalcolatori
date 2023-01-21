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

        String command, carattere, occ_s, dir, fileName;
        int occorrenze, res, nread, buffer_size = 4096;
        long numFile, fileLength;
        FileOutputStream fOutputStream;
        byte [] buffer = new byte[buffer_size];

        try {
            System.out.print("Inserire (C=ContaLineeConOccorrenze,B=TrasferisciFileBinari) : ");
            while (( command =  stdIn.readLine()) != null ) {

                if ( command.equalsIgnoreCase("C") ) {
                    // CONTEGGIO NUMERO LINEE CON OCCORRENZE
                    
                    outSock.writeUTF(command);

                    System.out.println("Inserire carattere: ");
                    carattere = stdIn.readLine();

                    System.out.println("Inserire numero di occorrenze: ");
                    occ_s = stdIn.readLine();

                    try {
                        occorrenze = Integer.parseInt(occ_s);
                    } catch (NumberFormatException e) {
                        System.out.println("L'input non è un numero");
                        System.out.print("Inserire (C=ContaLineeConOccorrenze,B=TrasferisciFileBinari) : ");
                        continue;
                    }

                    // SCRIVO CARATTERE E OCCORRENZE
                    outSock.writeChar(carattere.charAt(0));
                    outSock.writeInt(occorrenze);

                    // CHIUDO OUTPUT
                    //socket.shutdownOutput();

                    // RICEVO CARATTERI TROVATI
                    res = inSock.readInt();

                    System.out.println("Occorrenze del carattere trovato: " + res);

                }  else if ( command.equalsIgnoreCase("B") ) {
                    // LEGGO FILE BINARI DI UN DIRETTORIO
                    
                    outSock.writeUTF(command);

                    System.out.println("Inserire nome direttorio: ");
                    dir = stdIn.readLine();

                    // SCRIVO NOME DIRETTORIO
                    outSock.writeUTF(dir);
                    
                    // LEGGO SE DIRETTORIO ESISTE
                    if ( inSock.readBoolean() ) {

                        //RICEVO NUMERO FILE
                        numFile = inSock.readInt();
                        for ( int i = 0; i < numFile; i++ ) {
                            // LEGGO NOME FILE
                            fileName = inSock.readUTF();
        
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
                        

                    } else {
                        System.out.println("Dir inesistente o non valido");
                    }

                }
                System.out.print("Inserire (C=ContaLineeConOccorrenze,B=TrasferisciFileBinari) : ");
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
