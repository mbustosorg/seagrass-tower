
object EyeFrame {

  def main(args: Array[String]) = {
    val source = scala.io.Source.fromFile("frame2.txt")
    val lines: String = try {
      source.mkString
    } finally source.close()

    def nextByte(a: String): Any = {

      def padded(b: String): String = {
        if (b.length == 8) b
        else padded("0" + b)
      }

      if (!a.isEmpty) {
        val split = a.splitAt(a.length - 8)
        print("0b" + padded(split._2))
        if (a.length > 8) print(",")
        else println("")
        nextByte(split._1)
      }
    }

    val fullFrameText = lines.split("-").map(r =>
      {
        nextByte(r.split("\n").reverse.mkString)
      }
    )
  }

}