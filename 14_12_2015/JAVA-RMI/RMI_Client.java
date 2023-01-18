/**
 * Client.java
 * */

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
			String numPersone, tipoPrenotazione, id;

			System.out.print("Servizio (V=Visualizza, E=Elimina): ");

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("V")) {

					System.out.print("Numero minimo di persone? ");
					numPersone = stdIn.readLine();

					System.out.print("Tipo di prenotazione? ");
					tipoPrenotazione = stdIn.readLine();

					// Invocazione remota
					try {
						Prenotazione [] prenotazioni = serverRMI.visualizza_prenotazione(numPersone,tipoPrenotazione);
						if ( prenotazioni == null ) {
							System.out.println("Non ci sono prenotazioni con i criteri indicati o Errore");
						} else {
							System.out.println("ID | NUM_P | PRENOTAZIONE | VEICOLO | TARGA | IMMAGINE");
							for ( Prenotazione p : prenotazioni ) {
								if ( p != null ) {
									System.out.println(p.getId() + "|" + p.getNumPersone() + "|" + p.getTipoPrenotazione() + "|" 
										+ p.getTipoVeicolo() + "|" + p.getTarga() + "|" + p.getImmagine());
								}
							}
						}
						
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // C

				else
					if (service.equals("E")) {

						System.out.print("Id della prenotazione? ");
						id = stdIn.readLine();

						int res = 0;
						try {
							res = serverRMI.elimina_prenotazione(id);
							System.out.println("L'eliminazione della prenotazione con id:" +id+
								" ha dato come esito "+ res +"!\n");
						} catch (RemoteException re) {
							System.out.println("Errore remoto: " + re.toString());
						}
					} // S

					else System.out.println("Servizio non disponibile");

				System.out.print("Servizio (V=Visualizza, E=Elimina): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}