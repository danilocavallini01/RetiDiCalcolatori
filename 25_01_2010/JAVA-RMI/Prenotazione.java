import java.io.Serializable;
import java.util.Arrays;

public class Prenotazione implements Serializable{
    
    public static final int M = 6;

    private String titolo;
    private String [] copie;
    private int copieTotali;
    private int copieDisponibili;

    public Prenotazione(String titolo,  int copieTotali, int copieDisponibili) {
        this.titolo = titolo;
        
        copie = new String[6];

        for ( int i = 0; i < M; i++ ) {
            if ( i < copieTotali ) {
                copie[i] = "L";
            } else {
                copie[i] = "-1/-1/-1";
            }
        }

        this.copieTotali = copieTotali;
        this.copieDisponibili = copieDisponibili;
    }

    public String getTitolo() {
        return titolo;
    }

    public String[] getCopie() {
        return copie;
    }

    public int getCopieTotali() {
        return copieTotali;
    }

    public int getCopieDisponibili() {
        return copieDisponibili;
    }
    
    public boolean prestaLibro(int gg, int mm, int aa) {
        if ( this.copieDisponibili == 0 ) return false;

        for ( int i = 0; i < copieTotali; i++ ) {
            if ( copie[i].equals("L") ) {
                copie[i] = gg + "/" + mm + "/" + aa;
                this.copieDisponibili--;
                return true;
            }
        }

        return false;
    }

    @Override
    public String toString() {
        return "Prenotazione [titolo=" + titolo + ", copie=" + Arrays.toString(copie) + ", copieTotali=" + copieTotali
                + ", copieDisponibili=" + copieDisponibili + "]";
    }

    
}
