/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RMI_interfaceFile extends Remote {

	public int elimina_prenotazione(String targa) throws RemoteException;

	public Prenotazione[] visualizza_prenotazione(String tipoVeicolo) throws RemoteException;
}