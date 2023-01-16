
/**
 * ServerImple.java
 * 	Implementa i servizi dichiarati in RemOp.
 * */

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.List;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {
	
	public RMI_Server() throws RemoteException {
		super();
	}
	// Avvio del Server RMI
	public static void main(String[] args) {

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

	@Override
	public synchronized String[] lista_file(String nomeDirettorio) throws RemoteException {
		
		File dir = new File(nomeDirettorio);

		if ( !dir.exists() || !dir.isDirectory() || !dir.canRead() ) {
			throw new RemoteException("Direttorio inesistente o non leggibile");
		}

		if ( dir.listFiles().length == 0 ) {
			throw new RemoteException("Nessun file presente nel direttorio");
		}

		String tmp [] = new String[dir.listFiles().length];
		String result [];

		int i = 0, consonanti; 
		char fileName [];


		for ( File f : dir.listFiles() ) {
			fileName = f.getName().toLowerCase().toCharArray();
			consonanti = 0;

			
			for ( int j = 0; j < fileName.length; j++ ) {
				if ( fileName[j] != 'a' && fileName[j] != 'e' && fileName[j] != 'j' 
					&& fileName[j] != 'o' && fileName[j] != 'u' ) {

					
					consonanti++;
					if ( consonanti >= 3 ) {
						tmp[i++] = f.getName();
						break;
					}
				}
			}
		}

		result = new String[i];

		for ( i = 0; i < result.length; i++ ) {
			result[i] = tmp[i]; 
		}

		return result;
	}

	@Override
	public synchronized int numerazione_righe(String nomeFile) throws RemoteException {
		
		int result = 0, riga = 0;

		File f = new File(nomeFile);

		if ( !f.exists() || !f.isFile() || !f.canRead() ) {
			return -1;
		}

		try {
			BufferedReader br = new BufferedReader(new FileReader(f));

			while(br.readLine() != null ) {
				riga++;

				if ( riga % 2 != 0 ) {
					result++;
				}
			} 

			return result;

		} catch (FileNotFoundException e) {
			return -1;
		} catch (IOException e) {
			throw new RemoteException(e.getMessage());
		}
	}

}