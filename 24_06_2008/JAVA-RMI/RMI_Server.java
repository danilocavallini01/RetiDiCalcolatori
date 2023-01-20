import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	public RMI_Server() throws RemoteException {
		super();
	}

	@Override
	public String[] lista_nome_file_contenenti_parola_in_linea(String dir, String word) throws RemoteException {
		
		if ( dir == null || word == null) {
			throw new RemoteException("Null input");
		}

		File dirF = new File(dir);
		String [] result;
		
		char [] word_c = word.toCharArray();

		FileReader fr;
		int validFiles = 0, chr, word_ind = 0, i = 0;

		if ( !dirF.exists() || !dirF.isDirectory() || !dirF.canRead() ) {
			return null;
		}

		for ( File f : dirF.listFiles() ) {
			if ( f.isFile() && f.canRead()) {
				try {
					word_ind = 0;
					fr = new FileReader(f);
					
					while ((chr = fr.read()) > 0) {
						if ( chr == '\n' ) {
							word_ind = 0;
						}

						if ( chr == word_c[word_ind] ) {
							word_ind++;

							if ( word_ind == word_c.length ) {
								validFiles++;
								break;
							}
						} else {
							word_ind = 0;
						}

					}


				} catch (FileNotFoundException e) {
					throw new RemoteException(e.getMessage());
				} catch	(IOException e) {
					throw new RemoteException(e.getMessage());
				}
			}
		}

		result = new String[validFiles];

		for ( File f : dirF.listFiles() ) {
			if ( f.isFile() && f.canRead()) {
				try {
					word_ind = 0;
					fr = new FileReader(f);
					
					while ((chr = fr.read()) > 0) {
						if ( chr == '\n' ) {
							word_ind = 0;
						}

						if ( chr == word_c[word_ind] ) {
							word_ind++;

							if ( word_ind == word_c.length ) {
								result[i++] = f.getName();
								break;
							}
						} else {
							word_ind = 0;
						}

					}

					fr.close();

				} catch (FileNotFoundException e) {
					throw new RemoteException(e.getMessage());
				} catch	(IOException e) {
					throw new RemoteException(e.getMessage());
				}
			}
		}

		return result;
	}

	@Override
	public int conta_numero_linee(String file, String word) throws RemoteException {
		
		if ( file == null || word == null ) {
			throw new RemoteException("Null inputs");
		}

		File f = new File(file);
		int result = 0, chr, word_ind = 0;
		boolean word_in_line_found = false;
		char [] word_c = word.toCharArray();

		
		if ( !f.exists() || !f.isFile() || !f.canRead() ) {
			return -1;
		}

		
		FileReader fr;	
		try {
			fr = new FileReader(f);

			while ((chr = fr.read()) > 0) {
				
				if ( chr == '\n' ) {
					word_ind = 0;
					word_in_line_found = false;
				}
	
				if ( chr == word_c[word_ind] && !word_in_line_found) {
					word_ind++;
	
					if ( word_ind == word_c.length ) {
						word_in_line_found = true;
						word_ind = 0;
						result++;
					}
				}
	
			}
		} catch (IOException e ) {
			throw new RemoteException(e.getMessage());
		}

		try {
			fr.close();
		} catch (IOException e) {
			throw new RemoteException(e.getMessage());
		}

		return result;
	}

	// Avvio del Server RMI
	public static void main(String[] args) {
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