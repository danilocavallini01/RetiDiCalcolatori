/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RMI_interfaceFile extends Remote {

	public String[] lista_nome_file_contenenti_parola_in_linea(String dir, String word) throws RemoteException;
	public int conta_numero_linee(String file, String word) throws RemoteException;
}