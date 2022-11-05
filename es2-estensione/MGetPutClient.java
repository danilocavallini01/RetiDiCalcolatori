import java.net.*;
import java.io.*;

public class MGetPutClient {

	public static void main(String[] args) throws IOException {

		InetAddress addr = null;
		int port = -1;
		Long dimMin = 0l;

		File[] directories;
		File[] files;

		try {
			if (args.length >= 4) {
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
				dimMin = Long.parseLong(args[2]);
			} else {
				System.out.println("Usage: java MGetPutClient serverAddr serverPort dimensions [dir1 ... dirN]");
				System.exit(1);
			}

			if (port < 1024 || port > 65535) {
				System.out.println("Porta non valida");
				System.exit(2);
			}

		} catch (Exception e) {
			System.out.println("Problemi, i seguenti: ");
			e.printStackTrace();
			System.out.println("Usage: java MGetPutClient serverAddr serverPort dimensions [dir1 ... dirN]");
			System.exit(2);
		}

		directories = new File[args.length - 3];

		for (int i = 3; i < args.length; i++) {
			directories[i - 3] = new File(args[i]);
			if (!directories[i - 3].isDirectory()) {
				System.out.println("Dir non è una cartella");
				System.exit(2);
			}
		}

		Socket socket = null;
		FileInputStream inFile = null;
		DataInputStream inSock = null;
		DataOutputStream outSock = null;
		Long lunghezza;
		int numeroFile;

		String nomeFile, path;
		String absolute;
		String comando = null;

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

						// ricezione attiva/skip
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

			socket.close();
			System.out.println("\n --  TERMINO  --");
			System.out.println(">---------------------------------------<");


		} catch (Exception e) {

			System.err.println("Errore irreversibile, il seguente: ");
			e.printStackTrace();
			System.err.println("Chiudo!");
			System.exit(3);
		}
	}
}
