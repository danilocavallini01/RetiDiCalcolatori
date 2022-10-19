import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.Arrays;

public class DiscoveryServer {
    public static void main(String args[]) {

        int DServerPort = -1;
        DatagramSocket socket = null;
        DatagramPacket packet = null;
        byte[] buf = new byte[1000];

        if ( args.length == 0 ) {
            System.err.println("Sintassi errata : DiscoveryServer <portaDiscoveryServer> nomefile1 port1 ...nomefileN portN");
            System.exit(2);
        }

        int possibleServers = (args.length - 1) / 2;
        RowSwapServer rowSwapServers[] = new RowSwapServer[possibleServers];
        int activePort[] = new int[possibleServers];
        String activeFiles[] = new String[possibleServers];
        int activeServer = 0;

        try {
            DServerPort = Integer.valueOf(args[0]);
        } catch ( NumberFormatException e ) {
            System.err.println("Argomento <portaDiscoveryServer> non numerico : " + args[0]);
            e.printStackTrace();
            System.exit(2);
        }

        if ( DServerPort < 1024 || DServerPort > 65535 ) {
            System.err.println("<portaDiscoveryServer> porta con valore numerico errato : " + DServerPort);
            System.exit(3);
        }
        
        try {
            socket = new DatagramSocket(DServerPort);
            packet = new DatagramPacket(buf, buf.length);
        } catch (SocketException e) {
            e.printStackTrace();
            System.exit(3);
        }
        
        for ( int i = 1 ; i < args.length; i += 2) {
            String filename = args[i];
            int port = -1;
            BufferedReader br = null;

            try {
                port = Integer.valueOf(args[i + 1]);
            } catch( NumberFormatException e ) {
                System.err.println("Argomento <porta> non numerico : " + args[i + 1]);
                e.printStackTrace();
                System.exit(2);
            }

            try {
                br = new BufferedReader(new FileReader(filename));
            } catch (FileNotFoundException e ) {
                System.err.println("File non trovato: " + filename);
		        e.printStackTrace();
                System.exit(2);
            }
            
            if ( isPortAlreadyUsed(activePort,port,activeServer) ) {
                System.err.println("Porta già usata da un altro server : " + port);
                System.exit(2);
            }

            if ( isFileAlreadyOpen(activeFiles, filename, activeServer) ) {
                System.err.println("File gia precedentemente specificato : " + filename);
                System.exit(2);
            }

            activePort[activeServer] = port;
            activeFiles[activeServer] = filename;
            rowSwapServers[activeServer++] = new RowSwapServer(filename,br,port);
        }

        for ( int i = 0 ; i < activeServer; i++) {
            rowSwapServers[i].start();
        }

        System.out.println("Ascoltando richieste in arrivo");

        String richiesta;
        byte[] data;

       
        while(true) {
            try {
                packet.setData(buf);
                socket.receive(packet);
            }
            catch (IOException e) {
                System.err.println("Problemi nella ricezione del datagramma: " + e.getMessage());
                e.printStackTrace();
                continue;
            }

            try {
                richiesta = SocketUtility.computeReceive(packet);

                int rowServerPort = findRowServer(richiesta, activePort, activeFiles, activeServer);
                System.out.println("DS-" + DServerPort + " Ricevuto: " + richiesta + " rimando : " + rowServerPort);

                data = SocketUtility.prepareSend(String.valueOf(rowServerPort));

                packet.setData(data);
                socket.send(packet);

            } catch (Exception e ) {
                System.err.println("Problemi nella lettura della richiesta");
                e.printStackTrace();
                continue;
            }
        }

    }

    private static int findRowServer(String richiesta, int ports[], String filenames[], 
        int activeServers ) {
        for ( int i = 0 ; i < activeServers; i++) {
            if ( richiesta.equals(filenames[i]) ) return ports[i];
        }

        return -1;
    }

    public static boolean isPortAlreadyUsed(int ports[], int newPort, int activeServers) {
        for ( int i = 0 ; i < activeServers; i++) {
            if ( ports[i] == newPort) return true;
        }
        return false;
    }

    public static boolean isFileAlreadyOpen(String filenames[], String newFilename, int activeServers) {
        for ( int i = 0 ; i < activeServers; i++) {
            if ( filenames[i].equals(newFilename)) return true;
        }
        return false;
    }
}
