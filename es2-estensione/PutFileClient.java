// PutFileClient.java

import java.net.*;
import java.io.*;

public class PutFileClient {

	public static void main(String[] args) throws IOException {

		InetAddress addr = null;
		int port = -1;
		Long dimMin = 0l;

		File startDirectory = new File(args[2]);
		File[] dir = startDirectory.listFiles();

		try {
			if (args.length == 4) {
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
				dimMin = Long.parseLong(args[3]);
			} else {
				System.out.println("Usage: java PutFileClient serverAddr serverPort dir dimensions");
				System.exit(1);
			}

			if ( port < 1024 || port > 65535 ) {
				System.out.println("Porta non valida");
				System.exit(2);
			}

			startDirectory = new File(args[2]);
			if ( !startDirectory.isDirectory() ) {
				System.out.println("Dir non è una cartella");
				System.exit(2);
			}
			dir = startDirectory.listFiles();
		}
		catch (Exception e) {
			System.out.println("Problemi, i seguenti: ");
			e.printStackTrace();
			System.out.println("Usage: java PutFileClient serverAddr serverPort");
			System.exit(2);
		}

		Socket socket = null;
		FileInputStream inFile = null;
		DataInputStream inSock = null;
		DataOutputStream outSock = null;
		Long lunghezza;
		String nomeFile;
		String absolute;
		String comando = null;
	
		int i = -1;

		try {
			try {
				socket = new Socket(addr, port);
				socket.setSoTimeout(30000);
				System.out.println("Creata la socket: " + socket);

				inSock = new DataInputStream(socket.getInputStream());
				outSock = new DataOutputStream(socket.getOutputStream());
			} catch (IOException e) {
				System.exit(2);
			}

			while (i < dir.length - 1) {

				i++;
				nomeFile = dir[i].getName();
				absolute = dir[i].getAbsolutePath();
				lunghezza = dir[i].length();

				if ( lunghezza < dimMin ) {
					continue;
				}

				try {
					outSock.writeUTF(nomeFile);
					System.out.println("Inviato il nome del file " + nomeFile);
					System.out.println("Inizio la trasmissione di " + nomeFile);

					// ricezione attiva/skip
					comando = inSock.readUTF();
					System.out.println("Ricevuto da server: " + comando);

					if (comando.equals("attiva")) {
						inFile = new FileInputStream(absolute);
						System.out.println("Dimensione del file: " + dir[i].length());
						outSock.writeLong(dir[i].length());
						FileUtility.trasferisci_a_byte_file_binario(new DataInputStream(inFile), outSock, dir[i].length());
						inFile.close();
					}
					
				} catch (Exception e) {

					System.out.println("Problemi nell'invio del nome di " + nomeFile + ": ");
					e.printStackTrace();
					continue;
				}
			}

			socket.close();
			System.out.println("PutFileClient: termino...");

		} catch (Exception e) {

			System.err.println("Errore irreversibile, il seguente: ");
			e.printStackTrace();
			System.err.println("Chiudo!");
			System.exit(3);
		}
	}
}
