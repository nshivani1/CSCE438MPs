import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;

/*
String token = itr.nextToken();
                word.set(token);
                context.write(word, one);
                String time = "";
                if (token.equals("T") == true) {
                    if (itr.hasMoreTokens()) {
                        token = itr.nextToken(); //date
                        word.set(token);
                        context.write(word, one);
                        System.out.println(token);
                    }
                    if (itr.hasMoreTokens()) {
                        time = itr.nextToken(); //gettime
                        word.set(token);
                        context.write(word, one);
                        System.out.println(time);
                    }
                    System.out.println("First increment");
                    if (itr.hasMoreTokens()) {
                        itr.nextToken(); // U
                        word.set(token);
                        context.write(word, one);
                        System.out.println(token);
                    }
                    System.out.println("Second increment");
                    if (itr.hasMoreTokens()) {
                        itr.nextToken(); // twitter handle
                        word.set(token);
                        context.write(word, one);
                        System.out.println(token);
                    }
                    System.out.println("Third increment");
                    if (itr.hasMoreTokens()) {
                        itr.nextToken(); // W
                        word.set(token);
                        context.write(word, one);
                        System.out.println(token);
                    }
                    System.out.println("Fourth increment");
                    if (itr.hasMoreTokens()) {
                        sleep_str = itr.nextToken();
                        word.set(token);
                        context.write(word, one);
                        System.out.println(sleep_str);
                    }
                    while (sleep_str.equals("T") == false) {
                        if (sleep_str.equals("sleep") == true) {
                            if (time.indexOf(":") != -1) {
                                // word.set(temp);
                                // context.write(word, one);
                                String temp = "";
                                if (time.length() > 3) {
                                    temp = time.substring(0, 2);
                                }

                                System.out.println("B4 SWITCH ");
                                switch (temp) {
                                    case "00":
                                        word.set(temp);
                                        break;
                                    case "01":
                                        word.set(temp);
                                        break;
                                    case "02":
                                        word.set(temp);
                                        break;
                                    case "03":
                                        word.set(temp);
                                        break;
                                    case "04":
                                        word.set(temp);
                                        break;
                                    case "05":
                                        word.set(temp);
                                        break;
                                    case "06":
                                        word.set(temp);
                                        break;
                                    case "07":
                                        word.set(temp);
                                        break;
                                    case "08":
                                        word.set(temp);
                                        break;
                                    case "09":
                                        word.set(temp);
                                        break;
                                    case "11":
                                        word.set(temp);
                                        break;
                                    case "12":
                                        word.set(temp);
                                        break;
                                    case "13":
                                        word.set(temp);
                                        break;
                                    case "14":
                                        word.set(temp);
                                        break;
                                    case "15":
                                        word.set(temp);
                                        break;
                                    case "16":
                                        word.set(temp);
                                        break;
                                    case "17":
                                        word.set(temp);
                                        break;
                                    case "18":
                                        word.set(temp);
                                        break;
                                    case "19":
                                        word.set(temp);
                                        break;
                                    case "20":
                                        word.set(temp);
                                        break;
                                    case "21":
                                        word.set(temp);
                                        break;
                                    case "22":
                                        word.set(temp);
                                        break;
                                    case "23":
                                        word.set(temp);
                                        break;
                                    case "24":
                                        word.set(temp);
                                        break;
                                    default:
                                        temp = "error";
                                        break;
                                }
                            }
                            System.out.println("END");
                            if (itr.hasMoreTokens()) {
                                sleep_str = itr.nextToken();
                                word.set(token);
                                context.write(word, one);
                            }
                        }
                    }
                }
 */


public class WordCount2 {

    public static class TokenizerMapper
            extends Mapper<Object, Text, Text, IntWritable> {

        private final static IntWritable one = new IntWritable(1);
        private Text word = new Text();

        public void map(Object key, Text value, Context context
        ) throws IOException, InterruptedException {
            StringTokenizer itr = new StringTokenizer(value.toString());
            while (itr.hasMoreTokens()) {
                String token = itr.nextToken();
                String temp = "";
                if(token.equals("T") == true){
                    if(itr.hasMoreTokens()){
                        token = itr.nextToken(); //date
                        System.out.println(token);
                    }
                    if(itr.hasMoreTokens()){
                        token = itr.nextToken();
                        System.out.println(token);// time
                        if(token.length() > 3){
                            temp = token.substring(0,2);
                        }
                    }
                    if(itr.hasMoreTokens()){
                        token = itr.nextToken();
                        System.out.println(token);// U
                    }
                    if(itr.hasMoreTokens()){
                        token = itr.nextToken();
                        System.out.println(token);// link
                    }
                    if(itr.hasMoreTokens()){
                        token = itr.nextToken();
                        System.out.println(token);// W
                    }
                    boolean done = false;
                    while(!done){
                        if(itr.hasMoreTokens()){
                            token = itr.nextToken();
                            System.out.println(token);// next token after W
                        }
                        if(token.equals("sleep") == true){
                            done = true;
                            //switch statments
                            switch (temp) {
                                case "00":
                                    word.set(temp);
                                    break;
                                case "01":
                                    word.set(temp);
                                    break;
                                case "02":
                                    word.set(temp);
                                    break;
                                case "03":
                                    word.set(temp);
                                    break;
                                case "04":
                                    word.set(temp);
                                    break;
                                case "05":
                                    word.set(temp);
                                    break;
                                case "06":
                                    word.set(temp);
                                    break;
                                case "07":
                                    word.set(temp);
                                    break;
                                case "08":
                                    word.set(temp);
                                    break;
                                case "09":
                                    word.set(temp);
                                    break;
                                case "11":
                                    word.set(temp);
                                    break;
                                case "12":
                                    word.set(temp);
                                    break;
                                case "13":
                                    word.set(temp);
                                    break;
                                case "14":
                                    word.set(temp);
                                    break;
                                case "15":
                                    word.set(temp);
                                    break;
                                case "16":
                                    word.set(temp);
                                    break;
                                case "17":
                                    word.set(temp);
                                    break;
                                case "18":
                                    word.set(temp);
                                    break;
                                case "19":
                                    word.set(temp);
                                    break;
                                case "20":
                                    word.set(temp);
                                    break;
                                case "21":
                                    word.set(temp);
                                    break;
                                case "22":
                                    word.set(temp);
                                    break;
                                case "23":
                                    word.set(temp);
                                    break;
                                case "24":
                                    word.set(temp);
                                    break;
                                default:
                                    temp = "error";
                                    break;
                            }
                            context.write(word, one);
                        }
                        if(done == false && itr.hasMoreTokens()) {
                            token = itr.nextToken();
                        }
                        if (token.equals("T") == true) {
                            done = true;
                        }
                    }
                }
            }
        }
    }
    public static class IntSumReducer
            extends Reducer<Text, IntWritable, Text, IntWritable> {
        private IntWritable result = new IntWritable();

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
        ) throws IOException, InterruptedException {

            int sum = 0;
            for (IntWritable val : values) {
                sum += val.get();
            }
            result.set(sum);
            context.write(key, result);
        }
    }

    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        //FOR SECOND PART
        //conf.set("mapreduce.input.keyvaluelinerecordreader.key.value.separator", "\n\n");

        Job job = Job.getInstance(conf, "word count");
        job.setJarByClass(WordCount2.class);
        job.setMapperClass(TokenizerMapper.class);
        job.setCombinerClass(IntSumReducer.class);
        job.setReducerClass(IntSumReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}