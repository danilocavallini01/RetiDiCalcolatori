import java.net.*;
import java.io.*;

public class MGetPutClient {

	public static void main(String[] args) throws IOException {

		InetAddress addr = null;
		int port = -1;
		Long dimMin = 0l;

		File[] directories;

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

			FTPHandler.PUT(outSock,inSock,directories,dimMin);

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
