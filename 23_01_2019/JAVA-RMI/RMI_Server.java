
/**
 * ServerImple.java
 * 	Implementa i servizi dichiarati in RemOp.
 * */

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	private static final int MAX = 10;
	private static Prenotazione[] prenotazioni;

	public RMI_Server() throws RemoteException {
		super();
	}

	public synchronized int elimina_prenotazione(String targa) throws RemoteException {
		for ( int i = 0; i < MAX;  i++) {
			if (prenotazioni[i].getTarga().equals(targa)) {
				prenotazioni[i] = new Prenotazione();
				return 0;
			}
		}

		return -1;
	}

	public synchronized Prenotazione[] visualizza_prenotazione(String tipoVeicolo) throws RemoteException {

		if ( !tipoVeicolo.equals("camper") && !tipoVeicolo.equals("auto")) {
			throw new RemoteException("invalid tipoVeicolo");
		}

		Prenotazione[] found = new Prenotazione[MAX];
		int i = 0;
		String patente;

		for (Prenotazione p : prenotazioni) {
			patente = p.getPatente();

			if (patente.charAt(0) > 'E' || patente.charAt(0) == 'E' && patente.charAt(1) > 'D') {
				if (patente.equals(tipoVeicolo)) {
					found[i++] = p;
				}
			}
		}

		if ( i == 0 ) {
			return null;
		} else {
			Prenotazione [] result = new Prenotazione[i];
			for ( int j = 0; j < i; j++ ) {
				result[j] = result[j];
			}
		}

		return found;
	}

	// Avvio del Server RMI
	public static void main(String[] args) {

		prenotazioni = new Prenotazione[MAX];

		for ( int i = 0; i < MAX; i++ ) {
			prenotazioni[i] = new Prenotazione();
		}

		int registryPort = 1099;
		String registryHost = "localhost";
		String serviceName = "ServerRMI";
		
		// Controllo dei parametri della riga di comando
		if (args.length!=0 && args.length!=1) {
			System.out.println("Sintassi: ServerImpl [registryPort]");
			System.exit(1);
		}
		
		if (args.length==1) {
			try {
				// Aggiungere anche controllo porta valida, nel range di quelle usabili
				registryPort = Integer.parseInt(args[0]);
			} catch (Exception e) {
				System.out.println("Sintassi: ServerImpl [registryPort], registryPort");
				System.exit(2);
			}
		}
		// Registrazione del servizio RMI
		String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;
		try {
			RMI_Server serverRMI = new RMI_Server();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
		} catch( Exception e) {
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}

}