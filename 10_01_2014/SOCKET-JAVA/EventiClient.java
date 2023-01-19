import java.net.*;
import java.io.*;

public class EventiClient {

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

        String command, tipo, luogo, prezzo;
        int nEventi, prezzoInt;

        try {
            System.out.print("Inserire (V=EventiDiUnTipoELuogo,D=EventiSottoPrezzo) : ");
            while (( command =  stdIn.readLine()) != null ) {

                if ( command.equals("V") ) {
                    // VISUALIZZA EVENTI DI UN CERTO TIPO E LUOGO
                    
                    outSock.writeUTF(command);

                    System.out.println("Inserire tipo evento: ");
                    tipo = stdIn.readLine();

                    System.out.println("Inserire luogo evento: ");
                    luogo = stdIn.readLine();

                    // SCRIVO TIPO E LUOGO
                    outSock.writeUTF(tipo);
                    outSock.writeUTF(luogo);

                    // RICEVO NUMERO DI EVENTI TROVATI
                    nEventi = inSock.readInt();

                    if ( nEventi == 0 ) {
                        System.out.println("Nessun evento trovato");
                    } else {
                        System.out.println("Eventi Trovati");
                        for ( int i = 0; i < nEventi; i++ ) {
                            for ( int j = 0; j < 6; j++ ) {
                                System.out.print(inSock.readUTF() + "|");
                            }
                            System.out.println("");
                        }
                    }

                }  else if ( command.equals("D") ) {

                    // VISUALIZZA EVENTI SOTTO CERTO PREZZO
                    
                    outSock.writeUTF(command);

                    System.out.println("Inserire prezzo: ");
                    prezzo = stdIn.readLine();

                    try {
                        prezzoInt = Integer.parseInt(prezzo);
                    } catch (NumberFormatException e ) {
                        System.out.println("L'input non è un numero");
                        continue;
                    }

                    // SCRIVO PREZZO
                    outSock.writeInt(prezzoInt);

                    // RICEVO NUMERO DI EVENTI TROVATI
                    nEventi = inSock.readInt();

                    if ( nEventi == 0 ) {
                        System.out.println("Nessun evento trovato");
                    } else {
                        System.out.println("Eventi Trovati");
                        for ( int i = 0; i < nEventi; i++ ) {
                            for ( int j = 0; j < 6; j++ ) {
                                System.out.print(inSock.readUTF() + "|");
                            }
                            System.out.println("");
                        }
                    }

                }
                System.out.print("Inserire (V=EventiDiUnTipoELuogo,D=EventiSottoPrezzo) : ");
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
