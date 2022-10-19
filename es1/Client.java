import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Client {
    public static void main(String args[]) {

        DatagramSocket  socket = null;
        DatagramPacket packet = null;
        byte[] data = new byte[1000];
        InetAddress address = null;

        if ( args.length < 3 ) {
            System.err.println("Sintassi errata : <ipDS> <portaDs> <filename>");
            System.exit(2);
        }
        try {
            address = InetAddress.getByName(args[0]);
        } catch (UnknownHostException e1) {
            e1.printStackTrace();
            System.exit(3);
        }

        try {
            socket = new DatagramSocket();
            packet = new DatagramPacket(data, data.length, address , Integer.valueOf(args[1]));

        } catch (SocketException e) {
            e.printStackTrace();
            System.exit(3);
        }

        String richiesta;
       
        try {
            data = SocketUtility.prepareSend(args[2]);

            packet.setData(data);
            socket.send(packet);

            socket.receive(packet);
            richiesta = SocketUtility.computeReceive(packet);

            System.out.println("Porta del server RS (-1 non esiste) : " + richiesta);

            BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

            while (true) {
                System.out.print("Inserire due numeri di riga validi: ");

                
                String comando = stdIn.readLine();
                
                
                data = SocketUtility.prepareSend(comando);
    
                try {
                    packet = new DatagramPacket(data, data.length, address, Integer.valueOf(richiesta));
                    socket.send(packet);
                } catch ( NumberFormatException e ) {
                    System.err.println("Non hai inserito due numeri validi");
                }
    
                
                socket.receive(packet);


                richiesta = SocketUtility.computeReceive(packet);
                System.out.println("Esito dell operazione : " + richiesta );
            }       
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(2);
        }
    }
}
