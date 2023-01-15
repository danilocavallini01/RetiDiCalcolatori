import java.net.*;
import java.io.*;

public class RentACarClient {

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
                            "Usage: java RentACarClient serverAddr serverPort");
                    System.exit(1);
                }

            } else {
                System.out.println("Usage: java RentACarClient serverAddr serverPort");
                System.exit(1);
            }
        } catch (Exception e) {
            System.out.println("Problemi, i seguenti: ");
            e.printStackTrace();
            System.out.println("Usage: java RentACarClient serverAddr serverPort");
            System.exit(2);
        }

        // Variables for sockets
        Socket socket = null;
        DataInputStream inSock = null;
        DataOutputStream outSock = null;

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

        int buffer_size = 4096;

        String command, targa, fileName;
        boolean success;
        long fileLength;
        
        byte [] buffer = new byte[buffer_size];
        int nread, numFile;
        FileOutputStream donwloadedImage;

        try {
            System.out.print("Inserire (E=EliminaPrenotazione,D=DownloadImmagini) : ");
            while (( command =  stdIn.readLine()) != null ) {
                outSock.writeUTF(command);
    
                if ( command.equals("E") ) {
                    // ELIMINA
                    System.out.print("Inserire Targa: ");
                    targa = stdIn.readLine();
    
                    // INVIO TTARGA
                    outSock.writeUTF(targa);
                    // RICEZIONE RISULTATO
                    success = inSock.readBoolean();
    
                    if ( success ) {
                        System.out.println("Prenotazione rimossa con successo");
                    } else {
                        System.out.println("Errore!, Prenotazione non rimossa");
                    }
                }  else if ( command.equals("D") ) {
                    System.out.print("Inserire Targa: ");
                    targa = stdIn.readLine();
    
                    // INVIO TTARGA
                    outSock.writeUTF(targa);
                    // RICEZIONE RISULTATO
                    success = inSock.readBoolean();
                    
                    if ( success ) {
                        System.out.println("Cartella recuperata, inizio download...");
    
                        //RICEVO NUMERO FILE
                        numFile = inSock.readInt();
                        for ( int i = 0; i < numFile; i++ ) {
                            // LEGGO NOME FILE
                            fileName = inSock.readUTF();
        
                            // LEGGO LUNGHEZZA FILE
                            fileLength = inSock.readLong();
            
                            // SCRIVO BYTE SU FILE 
                            donwloadedImage = new FileOutputStream(fileName);
        
                            while ( fileLength > 0 ) {
                                nread = inSock.read(buffer, 0, fileLength > buffer_size ? buffer_size : (int)fileLength);
                                donwloadedImage.write(buffer, 0, nread);
        
                                fileLength -= nread;
                            }
        
                            donwloadedImage.close();
                        }
                        
                    } else {
                        System.out.println("Errore!, Cartella immagini non presente sul server");
                    }
                }
                System.out.print("Inserire (E=EliminaPrenotazione,D=DownloadImmagini) : ");
            }
    
            System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti il nome della cartella: ");

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
