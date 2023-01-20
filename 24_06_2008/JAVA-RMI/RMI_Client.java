import java.rmi.*;
import java.io.*;

class RMI_Client {

	public static void main(String[] args) {
		int registryPort = 1099;
		String registryHost = null;
		String serviceName = "ServerRMI";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo parametri
		if (args.length != 1 && args.length != 2) {
			System.out.println("Sintassi: RMI_Client RegistryHost [registryPort]");
			System.exit(1);
		} else {
			registryHost = args[0];
			if (args.length == 2) {
				try {
					registryPort = Integer.parseInt(args[1]);
				} catch (Exception e) {
					System.out
					.println("Sintassi: RMI_Client NomeHost [registryPort], registryPort intero");
					System.exit(1);
				}
			}
		}

		// Connessione al servizio RMI remoto
		try {
			String completeName = "//" + registryHost + ":" + registryPort + "/"
					+ serviceName;
			RMI_interfaceFile serverRMI = (RMI_interfaceFile) Naming.lookup(completeName);
			System.out.println("Client RMI: Servizio \"" + serviceName
					+ "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			String dir, word, file;

			System.out.print("Servizio (L=ListaFileContenentiParola,C=ContaNumeroLinee): ");

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("L")) {

					System.out.print("Nome del direttorio: ");
					dir = stdIn.readLine();

					System.out.print("Parola da cercare: ");
					word = stdIn.readLine();

					// Invocazione remota
					try {
						String [] result = serverRMI.lista_nome_file_contenenti_parola_in_linea(dir, word);

						if ( result == null ) {
							System.out.println("Errore o parametri errati");
						} else {
							System.out.println("File contenenti la parola sono");

							for ( String r : result ) {
								System.out.println(r);
							}
						}
						
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				}
				else
					if (service.equals("C")) {

						System.out.print("Nome del file: ");
						file = stdIn.readLine();

						System.out.print("Parola da cercare: ");
						word = stdIn.readLine();

						try {
							
							int res = serverRMI.conta_numero_linee(file, word);

							if ( res <= 0 ) {
								System.out.println("File non trovato o parola non trovata nel file");
							} else {
								System.out.println("Sono state trovate " + res + " occorrenze della parola");
							}

						} catch (RemoteException re) {
							System.out.println("Errore remoto: " + re.toString());
						}
					} // S

					else System.out.println("Servizio non disponibile");

					System.out.print("Servizio (L=ListaFileContenentiParola,C=ContaNumeroLinee): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}