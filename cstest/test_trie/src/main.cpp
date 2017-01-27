#include <cstdio>
#include <cstdlib>

#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QSet>
#include <QtCore/QStringList>

#include <csQt/csFlatTrie.h>
#include <csQt/csTrie.h>

#undef HAVE_REVERSE

template<typename T>
inline T reversed(const T& in)
{
  T out;
  if( in.isEmpty() ) {
    return out;
  }
  out.resize(in.size());
  for(int i = 0; i < in.size(); i++) {
    out[i] = in[in.size()-1-i];
  }
  return out;
}

typedef QSet<QString> Words;

Words readWords(const QString& filename)
{
  Words words;

  QFile file(filename);
  if( !file.open(QFile::ReadOnly) ) {
    fprintf(stderr, "ERROR: Unable to open file \"%s\"!\n",
            qPrintable(filename));
    return words;
  }

  const QString content = QString::fromUtf8(file.readAll());
  file.close();

  const QStringList split = content.split(QRegExp("\\W+"), QString::SkipEmptyParts);
  for(const QString& s : split) {
    words.insert(s);
  }

  return words;
}

int main(int argc, char **argv)
{
  const Words words = readWords(argv[1]);

  csTrie trie;
  for(const QString& w : words.values()) {
#ifndef HAVE_REVERSE
    trie.insert(w);
#else
    trie.insertReverse(w);
#endif
  }

  csFlatTrie flatTrie = trie.flattened();

  int cntTrie(0), cntFlatTrie(0);
  for(const QString& w : words.values()) {
#ifndef HAVE_REVERSE
    if( trie.find(w) == cs::ExactMatch ) {
      cntTrie++;
    }
    if( flatTrie.find(w) == cs::ExactMatch ) {
      cntFlatTrie++;
    }
#else
    if( trie.find(reversed(w)) == cs::ExactMatch ) {
      cntTrie++;
    }
    if( flatTrie.find(reversed(w)) == cs::ExactMatch ) {
      cntFlatTrie++;
    }
#endif
  }

  printf("***");
#ifdef HAVE_REVERSE
  printf(" REVERSE");
#endif
  printf(" FIND\n");
  printf("trie: %d of %d\n", cntTrie, words.size());
  printf("flat: %d of %d\n", cntFlatTrie, words.size());

#ifndef HAVE_REVERSE
  printf("*** COMPLETE\n");
  const QStringList result = flatTrie.complete("you");
  for(const QString& s : result) {
    printf("%s\n", qPrintable(s));
  }
#endif

  return EXIT_SUCCESS;
}
