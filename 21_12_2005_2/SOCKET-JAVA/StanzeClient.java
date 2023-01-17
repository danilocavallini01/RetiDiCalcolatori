import java.net.*;
import java.io.*;

public class StanzeClient {

    public static void main(String[] args) throws IOException {
        InetAddress addr = null;
        int port = -1;

        InetAddress dgramAddr = null;
        int dgramPort = -1;

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        
        try {
            if (args.length == 4) {
                addr = InetAddress.getByName(args[0]);
                port = Integer.parseInt(args[1]);
                if (port < 1024 || port > 65535) {
                    System.out.println(
                            "Usage: java StanzeClient hostServerStream portServerStream hostServerDatagram portServerDatagram");
                    System.exit(1);
                }

                dgramAddr = InetAddress.getByName(args[2]);
                dgramPort = Integer.parseInt(args[3]);
                if (dgramPort < 1024 || dgramPort > 65535) {
                    System.out.println(
                            "Usage: java StanzeClient hostServerStream portServerStream hostServerDatagram portServerDatagram");
                    System.exit(1);
                }

            } else {
                System.out.println("Usage: java StanzeClient hostServerStream portServerStream hostServerDatagram portServerDatagram");
                System.exit(1);
            }
        } catch (Exception e) {
            System.out.println("Problemi, i seguenti: ");
            e.printStackTrace();
            System.out.println("Usage: java StanzeClient hostServerStream portServerStream hostServerDatagram portServerDatagram");
            System.exit(2);
        }

        // SOCKET STREAM VARIABLES
        Socket socket = null;
        DataInputStream inSock = null;
        DataOutputStream outSock = null;

        // DATAGRAM SOCKET VARIABLES
        DatagramSocket datagramSocket = null;
		DatagramPacket datagramPacket = null;
		byte[] buf = new byte[256];
		try {
			datagramSocket = new DatagramSocket();
			datagramSocket.setSoTimeout(30000);
			datagramPacket = new DatagramPacket(buf, buf.length, dgramAddr, dgramPort);
			System.out.println("\nCreato la socket stream: avviato");
			System.out.println("Creata la socket: " + datagramSocket);
		}
		catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.out.println("StanzeClient: interrompo...");
			System.exit(1);
		}

        String command;

        // SOCKET VARIABLES
        int N;
        String stanza;

        // DGRAM VARIABLES
        ByteArrayOutputStream boStream = null;
		DataOutputStream doStream = null;;
		ByteArrayInputStream biStream = null;
		DataInputStream diStream = null;
		byte[] data = null;

        String nomeStanza;
        int risposta = -1;

        try {
            System.out.print("Inserire (V=VisualizzaStatoStanze,S=SospendiStanze) : ");
            while (( command =  stdIn.readLine()) != null ) {

                if ( command.equals("V") ) {
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
                    
                    // RICEVO NUMERO RIGHE
                    N = inSock.readInt();

                    for ( int i = 0; i < N; i++ ) {
                        stanza = inSock.readUTF();
                        System.out.println(stanza);
                    }
                }  else if ( command.equals("S") ) {

                    // SOCKET DATAGRAM
                    
                    try {
                        System.out.print("Inserire Nome della stanza: ");
                        nomeStanza = stdIn.readLine();
    
                        // riempimento e invio del pacchetto (vuoto)
                        try {
                            boStream = new ByteArrayOutputStream();
                            doStream = new DataOutputStream(boStream);
                            doStream.writeUTF(nomeStanza);
                            data = boStream.toByteArray();
                            datagramPacket.setData(data);
                            datagramSocket.send(datagramPacket);
                        }
                        catch (IOException e) {
                            System.out.println("Problemi nell'invio della richiesta: ");
                            e.printStackTrace();
                        }
            
                        try {// settaggio del buffer di ricezione
                            datagramPacket.setData(buf);
                            datagramSocket.receive(datagramPacket);
                            // sospensiva solo per i millisecondi indicati, dopo solleva una SocketException
                        }
                        catch (IOException e) {
                            System.out.println("Problemi nella ricezione del datagramma: ");
                            e.printStackTrace();
                        }
            
                        try {
                            biStream = new ByteArrayInputStream(datagramPacket.getData(), 0, datagramPacket.getLength());
                            diStream = new DataInputStream(biStream);
                            risposta = diStream.readInt();
                        }
                        catch (IOException e) {
                            System.out.println("Problemi nella lettura della risposta: ");
                            e.printStackTrace();
                        }
                        
                        if ( risposta == 0 ) {
                            System.out.println("Stanza sospesa con successo");
                        } else {
                            System.out.println("Errore, stanza non sospesa");
                        }

                    }
                    catch (Exception e) {
                        System.out.println("Problemi nella ricezione dal Server: esco...");
                        // si potrebbe gestire altrimenti l'eccezione, ad esempio tentando nuovamente
                        e.printStackTrace();
                        System.exit(5);
                    }
                }
                System.out.print("Inserire (V=VisualizzaStatoStanze,S=SospendiStanze): ");
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
