import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	private static final int MAX = 10;
	private static Prenotazione[] prenotazioni;

	public RMI_Server() throws RemoteException {
		super();
	}

	@Override
	public int inserisci_nuovo_libro(String titolo, int copieTotali) throws RemoteException {
		
		if ( titolo == null ) {
			return -1;
		}

		for ( int i = 0; i < MAX; i++ ) {
			if ( prenotazioni[i].getTitolo().equals("L") ) {
				prenotazioni[i] = new Prenotazione(titolo, copieTotali, copieTotali);

				for ( Prenotazione p: prenotazioni ) {
					System.out.println(p);
				}

				
				return 0;
			}
		}

		return -1;
	}

	@Override
	public int richiedi_prestito_libro(String titolo, int gg, int mm, int aa) throws RemoteException {
		if ( titolo == null || titolo.equals("L") || gg == -1 || mm == -1 || aa == -1) {
			return -3;
		}

		for ( int i = 0; i < MAX; i++ ) {
			if ( prenotazioni[i].getTitolo().equals(titolo)) {
				if ( prenotazioni[i].prestaLibro(gg, mm, aa) ) {
					for ( Prenotazione p: prenotazioni ) {
						System.out.println(p);
					}
			
					return 0;
				} else {
					return -2;
				}
			} 
		}

		return -1;
	}

	// Avvio del Server RMI
	public static void main(String[] args) {
		prenotazioni = new Prenotazione[MAX];

		for ( int i = 0; i < MAX; i++ ) {
			prenotazioni[i] = new Prenotazione("L",-1,-1);
		}

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