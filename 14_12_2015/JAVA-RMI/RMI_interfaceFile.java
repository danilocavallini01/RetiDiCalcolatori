/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RMI_interfaceFile extends Remote {

	public int elimina_prenotazione(String id) throws RemoteException;

	public Prenotazione[] visualizza_prenotazione(String numPersone, String tipoPrenotazione) throws RemoteException;
}