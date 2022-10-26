/* FileUtility.java */

import java.io.*;

public class FileUtility {

	/**
	 * Nota: sorgente e destinazione devono essere correttamente aperti e chiusi
	 * da chi invoca questa funzione.
	 *  
	 */
	static protected void trasferisci_a_byte_file_binario(DataInputStream src,
			DataOutputStream dest, Long dim) throws IOException {
	
		// ciclo di lettura da sorgente e scrittura su destinazione
	    int buffer;    
		Long i = 0l;
	    try {
	    	// esco dal ciclo all lettura di un valore negativo -> EOF
	    	// N.B.: la funzione consuma l'EOF
	    	while (i < dim) {
				buffer = src.read();
	    		dest.write(buffer);
				i++;
	    	}

			dest.flush();
	    }
	    catch (EOFException e) {
	    	System.out.println("Problemi, i seguenti: ");
	    	e.printStackTrace();
	    }
	}
}