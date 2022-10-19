import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.StringTokenizer;

public class RowSwapServer extends Thread {
    
    private String filename;
    private BufferedReader bf;
    private int port;
    private DatagramSocket socket;
    private DatagramPacket packet;
    private byte[] data;
    private String richiesta;

    public RowSwapServer(String filename, BufferedReader bf, int port) {
        this.filename = filename;
        this.bf = bf;
        this.port = port;   
        this.richiesta = null;
        this.data = new byte[1000];
    }

    public void run() {

        System.out.println("Nuovo server RowSwap su " + port + " che legge " + filename);

        try {
            socket = new DatagramSocket(port);
            packet = new DatagramPacket(data, data.length);

        } catch (SocketException e) {
            e.printStackTrace();
            System.exit(2);
        }

        packet.setData(data);

        while(true) {
    
            String richiesta;
            boolean success = false;
    
            try {       
                socket.receive(packet);
    
                richiesta = SocketUtility.computeReceive(packet);
                
                
                System.out.println("RS-" + port + " Ricevuto da client: " + richiesta);
    
                int rigaSwap1,rigaSwap2;
                
                StringTokenizer tokenizer = new StringTokenizer(richiesta, " ");
    
                rigaSwap1 = Integer.valueOf(tokenizer.nextToken());
                rigaSwap2 = Integer.valueOf(tokenizer.nextToken());
    
                String riga, writeOnFileRow;
    
                int i = 0;
    
                FileWriter fw = new FileWriter(".tmp_" + filename);
    
                while ((riga = bf.readLine()) != null)  {
                    writeOnFileRow = riga;
    
                    if ( i + 1 == rigaSwap1) {
                        writeOnFileRow = LineUtility.getLine(filename, rigaSwap2);
                    } 
    
                    if ( i + 1 == rigaSwap2) {
                        writeOnFileRow = LineUtility.getLine(filename, rigaSwap1);
                    }
    
                    i++;
                    writeOnFileRow += "\n";
                    fw.write(writeOnFileRow);
                }
    
                fw.close();
    
                Runtime.getRuntime().exec("mv .tmp_" + filename + " " + filename);
    
                success = true;
            } catch (IOException e) {
                e.printStackTrace();
            }
    
    
            if ( success ) {
                data = SocketUtility.prepareSend("1");
            } else {
                data = SocketUtility.prepareSend("-1");
            }
    
            packet.setData(data, 0 , data.length);
            try {
                socket.send(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }
}
