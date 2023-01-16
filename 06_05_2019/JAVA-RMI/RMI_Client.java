/**
 * Client.java
 * */

import java.rmi.*;
import java.util.Arrays;
import java.io.*;

class RMI_Client {

	public static void main(String[] args) {
		int registryPort = 1099;
		String registryHost = null;
		String serviceName = "ServerRMI";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo parametri
		if (args.length != 1 && args.length != 2) {
			System.out.println("Sintassi: ClientFile RegistryHost [registryPort]");
			System.exit(1);
		} else {
			registryHost = args[0];
			if (args.length == 2) {
				try {
					registryPort = Integer.parseInt(args[1]);
				} catch (Exception e) {
					System.out
					.println("Sintassi: ClientFile NomeHost [registryPort], registryPort intero");
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
			System.out.print("Servizio (L=ListaFile,D=RigheDispari): ");

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("L")) {

					String nomeDirettorio, result[];
					System.out.print("Nome del direttorio? ");
					nomeDirettorio = stdIn.readLine();

					// Invocazione remota
					try {
						result = serverRMI.lista_file(nomeDirettorio);
						
						if ( result == null ) {
							System.out.println("Errore, Non è stato recuperato nessun file dal direttorio");
						} else {
							System.out.println("File Nel Direttorio con >= 3 consonanti\n" + Arrays.toString(result));
						}
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				}

				else
					if (service.equals("D")) {

						String file;
						System.out.print("Nome del file? ");
						file = stdIn.readLine();

						int res = 0;
						try {
							res = serverRMI.numerazione_righe(file);

							if ( res == -1 ) {
								System.out.println("Errore durante calcolo righe, file mancante o non leggibile");
							} else {
								System.out.println("Sono state trovate " + res + " righe dispari");
							}
						} catch (RemoteException re) {
							System.out.println("Errore remoto: " + re.toString());
						}
					}

					else System.out.println("Servizio non disponibile");

				System.out.print("Servizio (L=ListaFile,D=RigheDispari): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}