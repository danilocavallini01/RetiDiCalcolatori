import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class Consumatore {

	public static void main(String[] args) {
		String prefix; 
		BufferedReader fd;
		Thread [] childs;

		if (args.length < 2){ 
			System.out.println("Utilizzo: consumatore <prefix> <filenames...>");
			System.exit(0);
		}

		prefix = args[0];
		childs = new Thread[args.length - 1];

		for ( int i = 1 ; i < args.length; i++ ) {
			try {
				fd = new BufferedReader(new FileReader(args[i]));
				childs[i - 1] = new readThread(args[i], fd, prefix);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				System.exit(3);
			}
		}

		for (Thread child : childs) {
			child.start();
		}
	}
}

class readThread extends Thread {
	private String filename;
	private BufferedReader reader;
	private String prefix;

	public readThread(String filename, BufferedReader reader, String prefix) {
		this.filename = filename;
		this.reader = reader;
		this.prefix = prefix;
	}

	public void run() {
		String finalString = this.filtraCaratteri(reader, prefix);
		try {
			FileWriter file_tmp = new FileWriter(".tmp_" + filename);

			file_tmp.write(finalString);
			file_tmp.close();
			reader.close();

			Runtime.getRuntime().exec("mv " + ".tmp_" + filename + " " + filename);
		} catch ( IOException e) {
			e.printStackTrace();
			System.exit(3);
		}
		
	}

	
	public String filtraCaratteri(BufferedReader in, String filterString) {
		
		String ret = "";
		String str;

		try {
			while( (str=in.readLine()) != null){
				for (int i=0; i < filterString.length(); i++) {
					ret += str.replaceAll("" + filterString.charAt(i), "") + "\n";
				}
			}
		} catch(IOException ex){
			System.out.println("Errore di input");
			System.exit(2);
		}
		return ret;
	}		
}
