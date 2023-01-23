/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RMI_interfaceFile extends Remote {

	public int inserisci_nuovo_libro(String titolo, int copieTotali) throws RemoteException;
	public int richiedi_prestito_libro(String titolo, int gg, int mm, int aa) throws RemoteException;
}