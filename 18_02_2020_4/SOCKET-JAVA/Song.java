import java.io.Serializable;

public class Song implements Serializable {
    private String singer;
    private String title;
    private int votes;
    private String fileName;

    public Song(String singer, String title, int votes, String fileName) {
        this.singer = singer;
        this.title = title;
        this.votes = votes;
        this.fileName = fileName;
    }

    public String getSinger() {
        return singer;
    }
    public String getTitle() {
        return title;
    }
    public int getVotes() {
        return votes;
    }
    public String getFileName() {
        return fileName;
    }

    
}
