#include <cstdio>
#include <cstdlib>

#include <csUtil/csFileIO.h>
#include <csUtil/csTextConverter.h>

void print(const std::string& s)
{
  for(int i = 0; i < s.size(); i++) {
    printf("%2d: 0x%02X\n", i, (uint8_t)s[i]);
  }
}

void print(const std::u16string& s)
{
  for(int i = 0; i < s.size(); i++) {
    printf("%2d: 0x%04X\n", i, (uint16_t)s[i]);
  }
}

void print_sep()
{
  printf("---\n");
}

int main(int /*argc*/, char ** /*argv*/)
{
  const csTextConverter c_ascii = csTextConverter::createAscii();
  const csTextConverter    c_l1 = csTextConverter::createLatin1();
  const csTextConverter    c_l9 = csTextConverter::createLatin9();
  const csTextConverter  c_utf8 = csTextConverter::createUtf8();
  const csTextConverter  c_1252 = csTextConverter::createWindows1252();

  const std::string utf8("für€");
  print(utf8); print_sep();

  const std::u16string utf16 = c_utf8.toUnicode(utf8);
  print(utf16); print_sep();

  const std::string ebit = c_1252.fromUnicode(utf16);
  print(ebit); print_sep();

  printf("*** File I/O\n");

  const std::string fileutf8 = csReadTextFile("../cstest/test_textconverter/test äuml.txt");
  print(fileutf8); print_sep();

  return EXIT_SUCCESS;
}
