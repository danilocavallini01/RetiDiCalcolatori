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

			String service, title;
			int totalCopies, result, gg, mm, aa;

			System.out.print("Servizio (I=InserisciLibro,R=RichiediPrestito): ");

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("I")) {

					System.out.print("Titolo del libro: ");
					title = stdIn.readLine();

					System.out.print("Copie totali: ");
					try {
						totalCopies = Integer.parseInt(stdIn.readLine());
					} catch ( NumberFormatException e ) {
						System.out.println("Numero non valido");
						System.out.print("Servizio (I=InserisciLibro,R=RichiediPrestito): ");
						continue;
					}

					// Invocazione remota
					try {
						
						result = serverRMI.inserisci_nuovo_libro(title, totalCopies);

						if ( result == -1 ) {
							System.out.println("Errore o parametri errati");
						} else {
							System.out.println("Libro aggiunto");
						}
						
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				}
				else
					if (service.equals("R")) {

						System.out.print("Titolo del libro: ");
						title = stdIn.readLine();

						System.out.print("Giorno: ");
						try {
							gg = Integer.parseInt(stdIn.readLine());
						} catch ( NumberFormatException e ) {
							System.out.println("Numero non valido");
							System.out.print("Servizio (I=InserisciLibro,R=RichiediPrestito): ");
							continue;
						}

						System.out.print("Mese: ");
						try {
							mm = Integer.parseInt(stdIn.readLine());
						} catch ( NumberFormatException e ) {
							System.out.println("Numero non valido");
							System.out.print("Servizio (I=InserisciLibro,R=RichiediPrestito): ");
							continue;
						}

						System.out.print("Anno: ");
						try {
							aa = Integer.parseInt(stdIn.readLine());
						} catch ( NumberFormatException e ) {
							System.out.println("Numero non valido");
							System.out.print("Servizio (I=InserisciLibro,R=RichiediPrestito): ");
							continue;
						}

						try {
							
							result = serverRMI.richiedi_prestito_libro(title, gg, mm, aa);

							if ( result == 0 ) {
								System.out.println("Prestito richiesto correttamente");
							} else if ( result == -1 ) {
								System.out.println("Titolo non presente");
							} else if ( result == -2 ) {
								System.out.println("Nessuna copia rimasta");
							} else {
								System.out.println("Parametri invalidi");
							}

						} catch (RemoteException re) {
							System.out.println("Errore remoto: " + re.toString());
						}
					} // S

					else System.out.println("Servizio non disponibile");

					System.out.print("Servizio (I=InserisciLibro,R=RichiediPrestito): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}