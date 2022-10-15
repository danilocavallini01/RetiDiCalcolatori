import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class Produttore {
	public static void main(String[] args) {		
		BufferedReader in = null;
		String inputl, write_on_file;
		ArrayList<FileWriter> fout = new ArrayList<FileWriter>();
		int i, index_doublepoints, file_number;

		if (args.length < 1){
			System.out.println("Utilizzo: produttore [ <inputFilenames> , ...]");
			System.exit(0);
		}

		try {
			for (i = 0; i < args.length; i++) {
				fout.add(new FileWriter(args[i]));
			}
		} catch (IOException e) {
			closeAllFiles(fout);
			e.printStackTrace();
			System.exit(3);
		}
		

		System.out.println("Inserire il contenuto del file fino a EOF");
		in = new BufferedReader(new InputStreamReader(System.in));
		
		try {
			while( (inputl = in.readLine()) != null) {

				index_doublepoints = inputl.indexOf(':');
				if ( index_doublepoints == -1 ) {
					closeAllFiles(fout);
					System.out.println(": necessari a inizio riga non trovati");
					System.exit(2);
				}
				
				file_number = Integer.valueOf(inputl.substring(0, index_doublepoints));
				write_on_file = inputl.substring(index_doublepoints+1);
				fout.get(file_number-1).write(write_on_file + "\n", 0, write_on_file.length()+1);
			}
			closeAllFiles(fout);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private static void closeAllFiles(ArrayList<FileWriter> files) {
		try {
			for ( FileWriter fout : files) {
				if ( fout != null ) {
					fout.close();
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}

