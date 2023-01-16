/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RMI_interfaceFile extends Remote {

	public String[] lista_file(String nomeDirettorio) throws RemoteException;

	public int numerazione_righe(String nomeFile) throws RemoteException;
}