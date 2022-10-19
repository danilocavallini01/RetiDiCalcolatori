import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;

public class SocketUtility {
    
    public static String computeReceive(DatagramPacket packet) {
        ByteArrayInputStream biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());;
		DataInputStream diStream = new DataInputStream(biStream);
        String richiesta = null;

        try {
            richiesta = diStream.readUTF();
        } catch (IOException e) {}

        return richiesta;
    }

    public static byte[] prepareSend(String message) {
        ByteArrayOutputStream boStream;
        DataOutputStream doStream;
        
        boStream = new ByteArrayOutputStream();
        doStream = new DataOutputStream(boStream);

        try {
            doStream.writeUTF(message);
        } catch (IOException e) {
            return null;
        }

        return boStream.toByteArray();
        
    }
}
