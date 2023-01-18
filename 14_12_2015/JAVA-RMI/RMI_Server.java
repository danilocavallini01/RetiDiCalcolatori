import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	private static final int MAX = 10;
	private static Prenotazione[] prenotazioni;

	public RMI_Server() throws RemoteException {
		super();
	}

	public synchronized int elimina_prenotazione(String id) throws RemoteException {

		if ( id.equals("L") ) {
			return -1;
		}

		for ( int i = 0; i < MAX;  i++) {
			if ( prenotazioni[i].getId().equals(id) ) {
				prenotazioni[i] = new Prenotazione("L","L","L","L","L","L");

				return 0;
			}
		}

		return -1;
	}

	public synchronized Prenotazione[] visualizza_prenotazione(String soglia, String tipoPrenotazione) throws RemoteException {
		
		if ( soglia.equals("L") || tipoPrenotazione.equals("L") ) {
			return null;
		}

		Prenotazione[] found = new Prenotazione[MAX];
		int i = 0, numPersone;

		int numPersoneMin = Integer.parseInt(soglia);


		for (Prenotazione p : prenotazioni) {
			if ( p.getId().equals("L") ) {
				continue;
			}

			numPersone = Integer.parseInt(p.getNumPersone());

			if ( p.getTipoPrenotazione().equals(tipoPrenotazione) 
				&& numPersone > numPersoneMin && p.getTipoVeicolo().equals("niente") ) {
				found[i++] = p;
			}
		}

		if ( i == 0 ) {
			return null;
		}

		Prenotazione [] result = new Prenotazione[i];
		for ( int j = 0; j < i; j++ ) {
			result[j] = found[j];
		}

		return found;
	}

	// Avvio del Server RMI
	public static void main(String[] args) {

		prenotazioni = new Prenotazione[MAX];

		for ( int i = 0; i < MAX; i++ ) {
			prenotazioni[i] = new Prenotazione("L","L","L","L","L","L");
		}

		prenotazioni[0] = new Prenotazione("GFD89","3","mezza piazzola","niente","L","mezza_piazz1.jpg");
		prenotazioni[1] = new Prenotazione("WERT26","5","piazzola deluxe","camper","AA567AA","piazz_deluxe2.jpg");
		int registryPort = 1099;
		String registryHost = "localhost";
		String serviceName = "ServerRMI";
		
		// Controllo dei parametri della riga di comando
		if (args.length!=0 && args.length!=1) {
			System.out.println("Sintassi: RMI_Server [registryPort]");
			System.exit(1);
		}
		
		if (args.length==1) {
			try {
				// Aggiungere anche controllo porta valida, nel range di quelle usabili
				registryPort = Integer.parseInt(args[0]);
			} catch (Exception e) {
				System.out.println("Sintassi: RMI_Server [registryPort], registryPort");
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