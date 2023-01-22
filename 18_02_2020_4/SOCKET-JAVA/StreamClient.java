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

        String command, title, fileName;
        long fileLength;
        FileWriter fw;
        char chr;
        int singersFound;

        try {
            System.out.print("Inserire (R=ReperimentoFileDaTitolo,V=Visualizza2Voti): ");
            while (( command =  stdIn.readLine()) != null ) {

                if ( command.equalsIgnoreCase("R") ) {
                    // REPERIMENTO DI FILE DA TITOLO
                    
                    outSock.writeUTF(command);

                    System.out.println("Inserire titolo canzone: ");
                    title = stdIn.readLine();

                    // SCRIVO TITOLO CANZONE
                    outSock.writeUTF(title);

                    // CONTROLLO SE TITOLO ESISTE
                    if ( !inSock.readBoolean()) {
                        System.out.println("Cantante non trovato nel registro");
                        System.out.print("Inserire (R=ReperimentoFileDaTitolo,V=Visualizza2Voti): ");
                        continue;
                    }

                    // CONTROLLO SE FILE ESISTE
                    if ( !inSock.readBoolean() ) {
                        System.out.println("File non trovato");
                        System.out.print("Inserire (R=ReperimentoFileDaTitolo,V=Visualizza2Voti): ");
                        continue;
                    }

                    // LEGGO NOME FILE
                    fileName = inSock.readUTF();
        
                    // LEGGO LUNGHEZZA FILE
                    fileLength = inSock.readLong();
    
                    // SCRIVO BYTE SU FILE 
                    fw = new FileWriter(new File(fileName));

                    while ( fileLength > 0 ) {
                        chr = inSock.readChar();
                        fw.write(chr);
                        System.out.print(chr);
                        fileLength--;
                    }
                    System.out.println();

                    fw.close();

                }  else if ( command.equalsIgnoreCase("V") ) {

                    // VISUALIZZA DUE CANZONI PIU VOTATE
                    outSock.writeUTF(command);

                    // LEGGO CANTANTI TROVATI
                    singersFound = inSock.readInt();

                    if ( singersFound == 0 ) {
                        System.out.println("Nessun cantante trovato");
                    } else if ( singersFound > 0 ) {
                        System.out.println("I " + singersFound + " cantanti con il voto piu' alto");
                        while ( singersFound > 0 ) {
                            // LEGGO CANTANTE
                            System.out.println(inSock.readUTF());
                            singersFound--;
                        }
                    }

                }
                System.out.print("Inserire (R=ReperimentoFileDaTitolo,V=Visualizza2Voti): ");
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
