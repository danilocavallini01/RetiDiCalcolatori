import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

public class ServerDgram {

	private static int PORT = -1;

	public static void main(String[] args) {

		/* Controllo argomenti */
		if ( args.length != 1) {
			System.out.println("Usage: java ServerDgram port"); 
			System.exit(1);
		}
		PORT = Integer.parseInt(args[0]);
		if (PORT <= 1024 || PORT > 65535) {
			System.out.println("The server port is not valid: " + args[0]);
			System.exit(2);
		}

		//Inizializzazione e apertura Socket
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];

		try {
			socket = new DatagramSocket(PORT);
			packet = new DatagramPacket(buf, buf.length);
			System.out.println("ServerDgram avviato con socket port: " + socket.getLocalPort()); 
		} catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(1);
		}

        // INIZIALIZZO STRUTTURA DATI
        Stanza [] stanze = null;
        final int N = 10;
        final int K = 8;

        stanze = new Stanza[N];
        String [] utenti;

        for ( int i = 0; i < N; i++ ) {
            utenti = new String[K];
        
            for ( int j = 0; j < K; j++ ) {
                utenti[j] = "L";
            }

            stanze[i] = new Stanza("L", "L", utenti);
        }

         
		try {
            
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;
			
			byte[] data = null;

            String nomeStanza;
            int risposta;

			while (true) {
				System.out.println("\nIn attesa di richieste...");

                // RICEZIONE NOME STANZA
				try {
					packet.setData(buf);
					socket.receive(packet);
				} catch (IOException e) {
					System.err.println("Problemi nella ricezione del datagramma: "+ e.getMessage());
					e.printStackTrace();
					continue;
				}

				try {
                    biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					nomeStanza = diStream.readUTF();
                    
					System.out.println("Richiesto server per stanza: " + nomeStanza);
				} catch (Exception e) {
					System.err.println("Problemi nella lettura della richiesta: ");
					e.printStackTrace();
					continue;
				}

                // ELABORO RISPOSTA
                risposta = -1;

                for ( int i = 0; i < N; i++ ) {
                    if ( stanze[i].getNome().equals(nomeStanza) && !stanze[i].getStato().contains("S")) {
                        stanze[i].setStato("S" + stanze[i].getStato());
                        risposta = 0;
                        break;
                    }
                }

                // INVIO RISPOSTA
				try {
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					doStream.writeInt(risposta);
					data = boStream.toByteArray();
					packet.setData(data, 0, data.length);
					socket.send(packet);
				} catch (IOException e) {
					System.err.println("Problemi nell'invio della risposta: "+ e.getMessage());
					e.printStackTrace();
					continue;
				}
				
			} // while
		}
		// qui catturo le eccezioni non catturate all'interno del while
		// in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("ServerDgram: termino...");
		socket.close();
	}
}