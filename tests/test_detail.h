#ifndef CMD_OPTIONS_TEST_TEST_DETAIL_H
#define CMD_OPTIONS_TEST_TEST_DETAIL_H

#include "cmd_options.h"

#include <iostream>
#include <iomanip>
#include <cmath>

namespace lemma {
namespace cmd_options {

template<typename CharT>
bool operator==(const basic_option_pack<CharT> &lhs,
  const basic_option_pack<CharT> &rhs)
{
  return lhs.value_provided == rhs.value_provided
    && lhs.prefix == rhs.prefix
    && lhs.raw_key == rhs.raw_key
    && lhs.packed_arguments == rhs.packed_arguments
    && lhs.value == rhs.value;
}

}
}

namespace co = lemma::cmd_options;

std::basic_ostream<char> & operator<<(std::basic_ostream<char> &out,
  const std::basic_string<char16_t> &str)
{
  return (out << co::detail::asUTF8(str));
}

std::basic_ostream<char> & operator<<(std::basic_ostream<char> &out,
  const std::basic_string<char32_t> &str)
{
  return (out << co::detail::asUTF8(str));
}

std::basic_ostream<char> & operator<<(std::basic_ostream<char> &out,
  const std::basic_string<wchar_t> &str)
{
  return (out << co::detail::asUTF8(str));
}


namespace detail {

#ifndef CMD_OPT_TEST_CHART
#define CMD_OPT_TEST_CHART char
#endif

#ifndef LITERAL_PREFIX
#define LITERAL_PREFIX
#endif

// call _LIT(...) to automatically add 'L' or other CharT literal
// qualifier to string literals
#define _CAT_LITERAL(x,y) x##y
#define _MAKE_LITERAL(lit,str) _CAT_LITERAL(lit,str)
#define _LIT(string) _MAKE_LITERAL(LITERAL_PREFIX,string)


typedef CMD_OPT_TEST_CHART check_char_t;

/*
  ASCII text only
*/
static const std::basic_string<check_char_t> ipsum(
  _LIT("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nisl libero, dignissim sed vulputate sit amet, fringilla a dui. Integer in velit ornare, hendrerit lectus lobortis, sodales elit. Maecenas vehicula lectus eu elit tincidunt, in eleifend elit faucibus. Aliquam cursus sed leo non lobortis. Curabitur ut rhoncus massa. Mauris ut lectus congue, ornare nisl eget, malesuada leo. Mauris eu tristique turpis. Etiam semper risus a diam rutrum, nec gravida leo blandit. Phasellus maximus convallis tincidunt.\n"));

/*
  ASCII text only
*/
static const std::basic_string<check_char_t> ipsum_par(
  _LIT("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce malesuada egestas scelerisque. Pellentesque a faucibus lectus. Morbi lobortis urna eget mi aliquet, in elementum quam aliquet. Duis pretium eros odio, a mollis odio tincidunt vel. Curabitur volutpat ipsum at pharetra pulvinar. Donec convallis, leo sed euismod auctor, velit urna varius velit, quis convallis justo elit rhoncus justo. Phasellus imperdiet tincidunt lorem, semper vehicula dolor vestibulum eu. Vestibulum lorem nunc, dapibus id sem non, aliquet fermentum lacus.\n  Pellentesque eleifend lacus quis turpis ultricies, vel convallis metus pharetra. Sed sagittis egestas justo eget pretium. Vestibulum sed diam vel libero auctor mollis ac at justo. Nullam vitae libero sed risus tincidunt condimentum. Donec non lacinia lectus. Aenean dignissim, risus et venenatis vestibulum, urna turpis tempor ipsum, sed tincidunt libero dui id velit. Phasellus feugiat sodales tellus non finibus."));

/*
  UTF but exceeds the basic multilingual plane, must use UTF-encoding
  and not UCS2 or wchar_t (fails on 2 byte versions, ie windows)
*/
#if 0
static const std::basic_string<check_char_t> utf_ipsum(
  _LIT("ꓡｏ𝙧ｅｍ 𝑖𝗽𝘀𝗎ｍ 𝒹٥l𝞼г 𝚜𝔦𝜏 ⍺ｍ𝒆𝒕, 𝖈ﻬ𝟉ƽе𝓬𝖙𝐞ᴛ𝞾ᴦ 𝖆ｄ𝘪ϱ𝞲𝚜ｃ𝐢𝖓ɡ 𝓮lꙇ𝚝. Ｆυ𝚜ｃ𝐞 ｍ𝔞lｅƽ𝝊𝛂𝕕𝒂 𝚎𝘨𝘦𝒔𝞃𝖺𝕤 𝐬𝓬ⅇlｅ𝓇𝗶𐑈𝙦ʋ𝐞. 𝙿ҽllе𝙣𝘁℮𝘀𝗊ʋе 𝝰 𝒻𝞪𝐮𝘤ι𝙗𝖚𝘀 l𝚎𝙘𝑡𝙪𝘴. 𝞛ەｒ𝖇ｉ lℴ𝒷੦𝑟𝓽ⅈ𝖘 𝓾𝓇𝘯𝕒 𝐞𝕘𝕖𝖙 ｍ𝗶 ⍺l𝗶𝔮𝓾𝖾𝞃, ℹ𝕟 ℮lҽｍ𝙚𝞏𝞃𝑢ｍ 𝙦ц𝞪ｍ 𝕒lͺｑ𝞾𝖊𝛕. 𝒟𝘂ɩѕ ρ𝙧𝒆𝐭𝗶𝖚ｍ 𝖊𝗋ﮫ𝒔 𝛐𝒹𝞲٥, 𝚊 ｍⲟllᎥ𐑈 𝚘ⅆｉᴏ 𝜏𝗂𝕟с𝗶𝗱𝔲𝔫𝞽 ∨ҽl. 𝐶𝕦ᴦ𝖆ｂ˛𝔱սг 𝘃ﮫl𝜐𝒕𝖕𝜶𝔱 ⍳𝕡𝒔𝚞ｍ 𝚊𝑡 𝘱𝗵𝚊ⲅ𝓮𝒕ⲅ𝙖 𝕡𝓊lν𝙞п𝑎ⲅ. 𝕯о𝝕ℯ𝖼 ᴄﻫ𝖓𝚟ɑllͺ𝖘, lℯہ 𝘀ⅇ𝖽 𝒆𝚞𝙞𝓈ｍ၀ⅆ 𝛼𝔲ⲥ𝘁ﻩ𝘳, ⋁ℯlӏ𝓉 𝝊𝐫𝜛𝒂 𝗏𝕒𝓻𝙞𝐮𝒔 𝓿ℯlι𝗍, 𝑞ᴜ𝑖𝚜 𝑐𝙤п𝕧𝛼llι𝐬 𝖏𝞾𝗌τ𝝈 ℯlі𝒕 𝓇𝕙ⲟℼ𝕔𝚞𝓈 јцｓ𝓽𝚘. Ⲣ𝗁𝐚𝘀𝖊ll𝙪𝖘 ͺｍ𝘱𝘦𝕣𝖉𝗶𝕖𝖙 𝝉ι𝓷ϲ𝚤𝖽ｕ𝞏𝐭 l०𝕣𝕖ｍ, 𝙨𝓮ｍ𝞺𝒆ⲅ 𝛎𝙚һ𝘪ｃ𝘂lａ 𝓭ᴑlﮦⲅ ᴠ𝗲𝐬𝘁і𝙗𝕦l𝒖ｍ ｅ𝙪. 𝑉ⅇѕ𝜏𝞲𝓫𝝊lυｍ lᴏ𝓻𝘦ｍ 𝓃սп𝒸, Ꮷ𝚊𝕡𝘪Ь𝛖𝓼 𝔦ⅾ ѕ𝘦ｍ 𝜛ο𝛡, 𝑎l𝜄𝔮𝘂ҽ𝘵 𝐟ҽгｍ𝒆𝑛𝞽𝒖ｍ l𝕒ｃᴜ𝚜.\𝗇 ℙⅇll𝐞𝕟𝒕𝚎𝒔𝖖ц𝓮 𝙚lе⍳𝔣𝖾𝜋𝒹 lɑ𝑐ᴜ𝙨 զ𝘶𝓲ѕ 𝞽ʋ𝙧𝘱ꙇ𝐬 𝗎l𝘵𝕣ι𝔠і𝖊𝙨, 𝜈𝓮l 𝗰𝝾𝙣ｖ𝕒ll𝔦ѕ ｍ𝒆τʋ𝗌 ｐ𝗵𝔞𝙧е𝛕𝒓а. 𝗦ℯᏧ 𝐬𝗮𝐠𝒊𝘵𝓽𝒾𝘀 𝑒𝑔𝚎ѕ𝜏𝙖𝐬 𝑗ｕ𝗌𝛕σ 𝘦𝗀е𝔱 𝗉г𝕖𝘁ι𝑢ｍ. 𝚅𝒆𝑠τ𝚤𝓫𝘂l𝗎ｍ 𝘀𝘦𝗱 𝖉𝖎аｍ ｖ𝖊l l𝐢𝖇𝑒𝑟ο α𝘶ⲥｔо𝑟 ｍⲟll𝚒𝖘 α𝕔 𝘢𝞃 ⅉ𝙪𐑈𝝉ⲟ. 𝖭𝚞ll𝓪ｍ 𝘷Ꭵ𝓉𝐚𝕖 lı𝐛ⅇ𝙧𝞼 𝚜𝐞ԁ 𝚛ℹ𝗌𝝊𝙨 𝗍ｉп𝖼ⅰ𝕕𝙪ℼ𝑡 𝗰๐𝘯𝙙ꙇｍ𝖾ᴨт𝜐ｍ. 𝓓ﮬո𝕖𝙘 𝐧οℼ l𝖺𝒸ı𝛡𝙞𝗮 l𝐞𐐽τ𝞾ѕ. 𝓐𝑒𝗇е⍺𝚗 ԁ𝖎𝔤𝙣𝚒𝒔𝓈𝜄ｍ, 𝐫𝝸ｓ𝒖ƽ 𝒆𝔱 𝔳еℼⅇ𝝕𝛼𝓉𝕚𝒔 𝘃𝖊ƽτ𝞲𝘣𝒖l𝖚ｍ, 𝙪ⲅ𝖓𝖺 𝕥𝑢𝐫𝔭𝚤ｓ 𝐭𝓮ｍрﻩ𝑟 𝒊ϱ𝑠𝘂ｍ, 𝓼𝕖ⅆ 𝞃𝜄𝐧𝓬𝑖𝐝𝞾𝙣𝛕 l𝗂Ƅｅᴦە 𝖽𝘶ɪ ͺｄ 𝝂𝖾l𝚤𝘵. 𝞠𝔥𝔞𝔰еll𝘂𝙨 𝒻ｅ𝖚𝙜𝑖𝐚𝗍 ｓﮭ𝙙𝗮lе𝘀 ᴛ𝓮llʋ𝓼 ℼە𝙣 𝖋і𝞹Ꭵ𝓫ʋｓ."));
#endif







/*
  All contents here are UTF coded in the basic multilingual plane. Each
  code point can be encoded as UTF8, UTF16 (as char16_t), UTF32
  (as char32_t), and as UCS2 (as 2 byte wchar_t and 4 byte wchar_t)
*/
static const std::basic_string<check_char_t> BMP_celebs(
_LIT("\u0540\u0561\u0575\u0561\u057d\u057f\u0561\u0576\u0020\u0531\u0580\u0561\u0574\u0020\u053d\u0561\u0579\u0561\u057f\u0580\u0575\u0561\u0576\u0020\u0041\u0075\u0073\u0074\u0072\u0061\u006c\u0069\u0061\u0020\u004e\u0069\u0063\u006f\u006c\u0065\u0020\u004b\u0069\u0064\u006d\u0061\u006e\u0020\u00d6\u0073\u0074\u0065\u0072\u0072\u0065\u0069\u0063\u0068\u0020\u004a\u006f\u0068\u0061\u006e\u006e\u0020\u0053\u0074\u0072\u0061\u0075\u00df\u0020\u0041\u007a\u0259\u0072\u0062\u0061\u0079\u0063\u0061\u006e\u0020\u0056\u0061\u0071\u0069\u0066\u0020\u0053\u0259\u006d\u0259\u0064\u006f\u011f\u006c\u0075\u0020\u0410\u0437\u04d9\u0440\u0431\u0430\u0458\u04b9\u0430\u043d\u0020\u0412\u0430\u0433\u0438\u0444\u0020\u0421\u04d9\u043c\u04d9\u0434\u043e\u0493\u043b\u0443\u0020\u0041\u007a\u0259\u0072\u0062\u0061\u0079\u0063\u0061\u006e\u0020\u0048\u0065\u0079\u0064\u0259\u0072\u0020\u018f\u006c\u0069\u0079\u0065\u0076\u0020\u0410\u0437\u04d9\u0440\u0431\u0430\u0458\u04b9\u0430\u043d\u0020\u04ba\u0435\u0458\u0434\u04d9\u0440\u0020\u04d8\u043b\u0438\u0458\u0435\u0432\u0020\u0042\u0065\u006c\u0067\u0069\u00eb\u0020\u0052\u0065\u006e\u00e9\u0020\u004d\u0061\u0067\u0072\u0069\u0074\u0074\u0065\u0020\u0042\u0065\u006c\u0067\u0069\u0071\u0075\u0065\u0020\u0052\u0065\u006e\u00e9\u0020\u004d\u0061\u0067\u0072\u0069\u0074\u0074\u0065\u0020\u0042\u0065\u006c\u0067\u0069\u0065\u006e\u0020\u0052\u0065\u006e\u00e9\u0020\u004d\u0061\u0067\u0072\u0069\u0074\u0074\u0065\u0020\u09ac\u09be\u0982\u09b2\u09be\u0020\u09b8\u09c1\u0995\u09c1\u09ae\u09be\u09b0\u0020\u09b0\u09be\u09af\u09bc\u0020\u0f60\u0f56\u0fb2\u0f74\u0f42\u0f0b\u0f61\u0f74\u0f63\u0f0d\u0009\u0020\u0f58\u0f42\u0f7c\u0f53\u0f0b\u0f54\u0f7c\u0f0b\u0f62\u0fa1\u0f7c\u0f0b\u0f62\u0f97\u0f7a\u0f0d\u0020\u1794\u17d2\u179a\u1791\u17c1\u179f\u1780\u1798\u17d2\u1796\u17bb\u1787\u17b6\u0020\u1796\u17d2\u179a\u17c7\u200b\u1796\u17bb\u1791\u17d2\u178b\u1783\u17c4\u179f\u17b6\u1785\u17b6\u179a\u200c\u17d2\u1799\u1787\u17bd\u1793\u178e\u17b6\u178f\u0020\u0043\u0061\u006e\u0061\u0064\u0061\u0020\u0043\u00e9\u006c\u0069\u006e\u0065\u0020\u0044\u0069\u006f\u006e\u0020\u14c4\u14c7\u1557\u14bb\u14a5\u1405\u1466\u0020\u14f1\u14f4\u14d0\u0020\u140a\u14a1\u14d7\u1483\u1472\u1585\u0020\u13e3\u13b3\u13a9\u0020\u13cd\u13cf\u13c9\u13ef\u0020\u4e2d\u56fd\u0020\u7ae0\u5b50\u6021\u0020\u4e2d\u56fd\u0020\u738b\u83f2\u0020\u010c\u0065\u0073\u006b\u006f\u0020\u0041\u006e\u0074\u006f\u006e\u00ed\u006e\u0020\u0044\u0076\u006f\u0159\u00e1\u006b\u0020\u0044\u0061\u006e\u006d\u0061\u0072\u006b\u0020\u0053\u00f8\u0072\u0065\u006e\u0020\u0048\u0061\u0075\u0063\u0068\u002d\u0046\u0061\u0075\u0073\u0062\u00f8\u006c\u006c\u0020\u0044\u0061\u006e\u006d\u0061\u0072\u006b\u0020\u0053\u00f8\u0072\u0065\u006e\u0020\u004b\u0069\u0065\u0072\u006b\u0065\u0067\u00e5\u0072\u0064\u0020\u0645\u0635\u0631\u0020\u0639\u0628\u062f\u0627\u0644\u062d\u0644\u064a\u0645\u0020\u062d\u0627\u0641\u0638\u0020\u0645\u0635\u0631\u0020\u0623\u0645\u0020\u0643\u0644\u062b\u0648\u0645\u0020\u12a4\u122d\u1275\u122b\u0020\u1265\u122d\u1203\u1290\u0020\u12d8\u122d\u12a3\u12ed\u0020\u12a2\u1275\u12ee\u1335\u12eb\u0020\u1283\u12ed\u120c\u0020\u1308\u1265\u1228\u1225\u120b\u1234\u0020\u0053\u0075\u006f\u006d\u0069\u0020\u004d\u0069\u006b\u0061\u0020\u0048\u00e4\u006b\u006b\u0069\u006e\u0065\u006e\u0020\u0046\u0072\u0061\u006e\u0063\u0065\u0020\u0047\u00e9\u0072\u0061\u0072\u0064\u0020\u0044\u0065\u0070\u0061\u0072\u0064\u0069\u0065\u0075\u0020\u0046\u0072\u0061\u006e\u0063\u0065\u0020\u004a\u0065\u0061\u006e\u0020\u0052\u00e9\u006e\u006f\u0020\u0046\u0072\u0061\u006e\u0063\u0065\u0020\u0043\u0061\u006d\u0069\u006c\u006c\u0065\u0020\u0053\u0061\u0069\u006e\u0074\u002d\u0053\u0061\u00eb\u006e\u0073\u0020\u0046\u0072\u0061\u006e\u0063\u0065\u0020\u004d\u0079\u006c\u00e8\u006e\u0065\u0020\u0044\u0065\u006d\u006f\u006e\u0067\u0065\u006f\u0074\u0020\u0046\u0072\u0061\u006e\u0063\u0065\u0020\u0046\u0072\u0061\u006e\u00e7\u006f\u0069\u0073\u0020\u0054\u0072\u0075\u0066\u0066\u0061\u0075\u0074\u0020\u10e1\u10d0\u10e5\u10d0\u10e0\u10d7\u10d5\u10d4\u10da\u10dd\u0020\u10d4\u10d3\u10e3\u10d0\u10e0\u10d3\u0020\u10e8\u10d4\u10d5\u10d0\u10e0\u10d3\u10dc\u10d0\u10eb\u10d4\u0020\u0044\u0065\u0075\u0074\u0073\u0063\u0068\u006c\u0061\u006e\u0064\u0020\u004a\u00fc\u0072\u0067\u0065\u006e\u0020\u004b\u006c\u0069\u006e\u0073\u006d\u0061\u006e\u006e\u0020\u0044\u0065\u0075\u0074\u0073\u0063\u0068\u006c\u0061\u006e\u0064\u0020\u0057\u0061\u006c\u0074\u0065\u0072\u0020\u0053\u0063\u0068\u0075\u006c\u0074\u0068\u0065\u0069\u00df\u0020\u0395\u03bb\u03bb\u03ac\u03c2\u0020\u0393\u03b9\u03ce\u03c1\u03b3\u03bf\u03c2\u0020\u039d\u03c4\u03b1\u03bb\u03ac\u03c1\u03b1\u03c2\u0020\u004d\u0061\u0067\u0079\u0061\u0072\u006f\u0072\u0073\u007a\u00e1\u0067\u0020\u0041\u006e\u0064\u0072\u00e1\u0073\u0020\u0053\u00fc\u0074\u0151\u0020\u00cd\u0073\u006c\u0061\u006e\u0064\u0020\u0042\u006a\u00f6\u0072\u006b\u0020\u0047\u0075\u00f0\u006d\u0075\u006e\u0064\u0073\u0064\u00f3\u0074\u0074\u0069\u0072\u0020\u092d\u093e\u0930\u0924\u0020\u092e\u093e\u0927\u0941\u0930\u0940\u0020\u0926\u0940\u0915\u094d\u0937\u093f\u0924\u0020\u0627\u06cc\u0631\u0627\u0646\u0020\u0645\u062d\u0633\u0646\u0020\u0645\u062e\u0645\u0644\u0628\u0627\u0641\u0020\u00c9\u0069\u0072\u0065\u0020\u0053\u0069\u006e\u00e9\u0061\u0064\u0020\u004f\u2019\u0043\u006f\u006e\u006e\u006f\u0072\u0020\u05d9\u05e9\u05e8\u05d0\u05dc\u0020\u05d9\u05d4\u05d5\u05e8\u05dd\u0020\u05d2\u05d0\u05d5\u05df\u0020\u0049\u0074\u0061\u006c\u0069\u0061\u0020\u0046\u0061\u0062\u0072\u0069\u007a\u0069\u006f\u0020\u0044\u0065\u0020\u0041\u006e\u0064\u0072\u00e9\u0020\u65e5\u672c\u0020\u4e45\u4fdd\u7530\u0020\u0020\u0020\u0020\u5229\u4f38\u0020\u65e5\u672c\u0020\u6797\u539f\u0020\u3081\u3050\u307f\u0020\u65e5\u672c\u0020\u5bae\u5d0e\u3000\u99ff\u0020\u65e5\u672c\u0020\u68ee\u9dd7\u5916\u0020\u65e5\u672c\u0020\u30c6\u30af\u30b9\u30fb\u30c6\u30af\u30b5\u30f3\u0020\uc870\uc120\u0020\ubbfc\uc8fc\uc8fc\uc758\u0020\uc778\ubbfc\u0020\uacf5\ud654\uad6d\u0020\uc774\uc124\ud76c\u0020\ub300\ud55c\ubbfc\uad6d\u0020\uc548\uc131\uae30\u0020\ub300\ud55c\ubbfc\uad6d\u0020\uc2ec\uc740\ud558\u0020\uc870\uc120\u0020\uc185\uc870\u11f0\u0020\u0028\uc138\uc885\ub300\uc655\u0029\u0020\u0052\u0065\u0070\u0075\u0062\u0062\u006c\u0069\u006b\u0061\u0020\u0074\u0061\u0027\u0020\u004d\u0061\u006c\u0074\u0061\u0020\u0054\u0072\u0065\u0076\u006f\u0072\u0020\u017b\u0061\u0068\u0072\u0061\u0020\u004e\u006f\u0072\u0067\u0065\u0020\u0054\u006f\u0072\u0020\u00c5\u0067\u0065\u0020\u0042\u0072\u0069\u006e\u0067\u0073\u0076\u00e6\u0072\u0064\u0020\u004e\u006f\u0072\u0065\u0067\u0020\u0048\u0065\u0072\u0062\u006a\u00f8\u0072\u006e\u0020\u0053\u00f8\u0072\u0065\u0062\u00f8\u0020\u067e\u0627\u06a9\u0633\u062a\u0627\u0646\u0020\u0646\u0635\u0631\u062a\u0020\u0641\u062a\u062d\u0020\u0639\u0644\u06cc\u0020\u062e\u0627\u0646\u0020\u0050\u0065\u0072\u00fa\u0020\u004e\u0069\u0063\u00f3\u006d\u0065\u0064\u0065\u0073\u0020\u0053\u0061\u006e\u0074\u0061\u0020\u0043\u0072\u0075\u007a\u0020\u0050\u006f\u006c\u0073\u006b\u0061\u0020\u004c\u0065\u0063\u0068\u0020\u0057\u0061\u0142\u0119\u0073\u0061\u0020\u0050\u006f\u0072\u0074\u0075\u0067\u0061\u006c\u0020\u0041\u006d\u00e1\u006c\u0069\u0061\u0020\u0052\u006f\u0064\u0072\u0069\u0067\u0075\u0065\u0073\u0020\u0050\u0075\u0065\u0072\u0074\u006f\u0020\u0052\u0069\u0063\u006f\u0020\u004f\u006c\u0067\u0061\u0020\u0054\u0061\u00f1\u00f3\u006e\u0020\u0052\u014d\u006d\u0061\u0020\u0050\u016b\u0062\u006c\u0069\u0075\u0073\u0020\u0043\u006f\u0072\u006e\u0113\u006c\u0069\u0075\u0073\u0020\u0053\u0063\u012b\u0070\u0069\u014d\u0020\u0041\u0066\u0072\u0069\u0063\u0101\u006e\u0075\u0073\u0020\u0420\u043e\u0441\u0441\u0438\u044f\u0020\u041c\u0438\u0445\u0430\u0438\u043b\u0020\u0413\u043e\u0440\u0431\u0430\u0447\u0451\u0432\u0020\u0420\u043e\u0441\u0441\u0438\u044f\u0020\u0411\u043e\u0440\u0438\u0441\u0020\u0413\u0440\u0435\u0431\u0435\u043d\u0449\u0438\u043a\u043e\u0432\u0020\u05e8\u05d5\u05e1\u05dc\u05d0\u05b7\u05e0\u05d3\u0020\u05e9\u05dc\u05d5\u05dd\u0020\u05e2\u05dc\u05d9\u05db\u05dd\u0020\u0053\u00e1\u0070\u006d\u0069\u0020\u00c1\u0069\u006c\u006c\u006f\u0068\u0061\u0161\u0020\u0053\u006c\u006f\u0076\u0065\u006e\u0073\u006b\u006f\u0020\u013d\u0075\u0064\u006f\u0076\u00ed\u0074\u0020\u0160\u0074\u00fa\u0072\u0020\u0053\u006c\u006f\u0076\u0065\u006e\u0069\u006a\u0061\u0020\u0046\u0072\u0061\u006e\u0065\u0020\u004d\u0069\u006c\u010d\u0069\u006e\u0073\u006b\u0069\u0020\u002d\u0020\u004a\u0065\u017e\u0065\u006b\u0020\u0053\u0076\u0065\u0072\u0069\u0067\u0065\u0020\u0042\u006a\u00f6\u0072\u006e\u0020\u0042\u006f\u0072\u0067\u0020\u03a3\u03c5\u03c1\u03b1\u03ba\u03bf\u03cd\u03c3\u03b1\u0020\u1f08\u03c1\u03c7\u03b9\u03bc\u03ae\u03b4\u03b7\u03c2\u0020\u062a\u0627\u062c\u064a\u0643\u0633\u062a\u0627\u0646\u0020\u0635\u062f\u0631\u0020\u0627\u0644\u062f\u064a\u0646\u0020\u0639\u064a\u0646\u064a\u0020\u0422\u043e\u04b7\u0438\u043a\u0438\u0441\u0442\u043e\u043d\u0020\u0421\u0430\u0434\u0440\u0438\u0434\u0434\u0438\u043d\u0020\u0410\u0439\u043d\u04e3\u0020\u0ba4\u0bae\u0bbf\u0bb4\u0bcd\u0020\u0ba8\u0bbe\u0b9f\u0bc1\u0020\u0b9a\u0bbf\u0bb5\u0bbe\u0b9c\u0bbf\u0020\u0b95\u0ba9\u0bc7\u0b9a\u0ba9\u0bcd\u0020\u81fa\u7063\u0020\u8212\u6dc7\u0020\u81fa\u7063\u0020\u674e\u5b89\u0020\u0e1b\u0e23\u0e30\u0e40\u0e17\u0e28\u0e44\u0e17\u0e22\u0020\u0e18\u0e07\u0e44\u0e0a\u0e22\u0020\u0e41\u0e21\u0e47\u0e04\u0e2d\u0e34\u0e19\u0e44\u0e15\u0e22\u0e4c\u0020\u0423\u043a\u0440\u0430\u0457\u043d\u0430\u0020\u041d\u0456\u043d\u0430\u0020\u041c\u0430\u0442\u0432\u0456\u0454\u043d\u043a\u043e\u0020\u0055\u002e\u0053\u002e\u0041\u002e\u0020\u0042\u0072\u0061\u0064\u0020\u0050\u0069\u0074\u0074\u0020\u040e\u0437\u0431\u0435\u043a\u0438\u0441\u0442\u043e\u043d\u0020\u0421\u043e\u043b\u0438\u0436\u043e\u043d\u0020\u0428\u0430\u0440\u0438\u043f\u043e\u0432\u0020\u004f\u0027\u007a\u0062\u0065\u006b\u0069\u0073\u0074\u006f\u006e\u0020\u0041\u006c\u0069\u0073\u0068\u0065\u0072\u0020\u0069\u0062\u006e\u0020\u0047\u0027\u0069\u0079\u006f\u0073\u0069\u0064\u0064\u0069\u006e\u0020\u004e\u0061\u0076\u006f\u0069\u0079\u0020\u0056\u0069\u1ec7\u0074\u0020\u004e\u0061\u006d\u0020\u0054\u0072\u1ecb\u006e\u0068\u0020\u0043\u00f4\u006e\u0067\u0020\u0053\u01a1\u006e\u0020\u004a\u0075\u0067\u006f\u0073\u006c\u0061\u0076\u0069\u006a\u0061\u0020\u0110\u006f\u0072\u0111\u0065\u0020\u0042\u0061\u006c\u0061\u0161\u0065\u0076\u0069\u0107\u0020\u0408\u0443\u0433\u043e\u0441\u043b\u0430\u0432\u0438\u0458\u0430\u0020\u0402\u043e\u0440\u0452\u0435\u0020\u0411\u0430\u043b\u0430\u0448\u0435\u0432\u0438\u045b"));






template<typename CharT>
struct std_stream_select;

template<>
struct std_stream_select<char> {
  static constexpr std::basic_ostream<char> &cout = std::cout;
  static constexpr std::basic_ostream<char> &cerr = std::cerr;
};

template<>
struct std_stream_select<char16_t> {
  static constexpr std::basic_ostream<char> &cout = std::cout;
  static constexpr std::basic_ostream<char> &cerr = std::cerr;
};

template<>
struct std_stream_select<char32_t> {
  static constexpr std::basic_ostream<char> &cout = std::cout;
  static constexpr std::basic_ostream<char> &cerr = std::cerr;
};

template<>
struct std_stream_select<wchar_t> {
  static constexpr std::basic_ostream<wchar_t> &cout = std::wcout;
  static constexpr std::basic_ostream<wchar_t> &cerr = std::wcerr;
};

template<typename CharT>
inline bool is_empty(const co::basic_option_description<CharT> &desc)
{
  return (
    !desc.unpack_option && !desc.mapped_key &&
    !desc.key_description && !desc.extended_description &&
    !desc.implicit_value &&
    !desc.implicit_value_description && !desc.make_value &&
    !desc.finalize);
}

/*
  Aligned to cmd_options.xlsx starting at row 2
*/
template<typename CharT>
inline bool is_mapped_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && desc.key_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_hidden_mapped_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && !desc.key_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_raw_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && desc.key_description
    && !desc.make_value);
}

/*
  Row 5
*/
template<typename CharT>
inline bool is_hidden_raw_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && !desc.key_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_mapped_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && desc.key_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_hidden_mapped_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && !desc.key_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && desc.key_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_mapped_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && !desc.key_description
    && desc.make_value && !desc.implicit_value);
}

/*
  Row 10
*/
template<typename CharT>
inline bool is_mapped_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && desc.key_description
    && desc.make_value && desc.implicit_value);
}

template<typename CharT>
inline bool is_hidden_mapped_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && !desc.key_description
    && desc.make_value && desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && desc.key_description
    && desc.make_value && desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_mapped_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && !desc.key_description
    && desc.make_value && desc.implicit_value);
}

/*
  In the EZ interface for row 14, 15, 18, and 19 mapped key is set to
  allow for position and argument number constraints. That is row 14 and
  row 15 are actually another occurrences of row 16 and row 17 and row
  18 and 19 are another occurrences of row 20 and 21. The mapped key
  simply provides the raw key which is the default behavior if it wasn't
  present. So this check is not used and here for completeness only.
*/
template<typename CharT>
inline bool is_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

/*
  Row 15
*/
template<typename CharT>
inline bool is_hidden_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && !desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_keyed_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_hidden_keyed_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && !desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_empty_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && desc.extended_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_hidden_empty_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && !desc.extended_description
    && !desc.make_value);
}

/*
  Row 20
*/
template<typename CharT>
inline bool is_empty_keyed_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && desc.extended_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_hidden_empty_keyed_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && !desc.extended_description
    && !desc.make_value);
}











template<typename CharT>
inline std::string
to_string(const co::basic_option_description<CharT> &desc)
{
  std::stringstream out;
  out
    << "unpack_option: " << bool(desc.unpack_option) << "\n"
    << "mapped_key: " << bool(desc.mapped_key) << "\n"
    << "key_description: " << bool(desc.key_description) << "\n"
    << "extended_description: " << bool(desc.extended_description) << "\n"
    << "implicit_value: " << bool(desc.implicit_value) << "\n"
    << "implicit_value_description: "
      << bool(desc.implicit_value_description) << "\n"
    << "make_value: " << bool(desc.make_value) << "\n"
    << "finalize: " << bool(desc.finalize) << "\n";

  return out.str();
}

template<typename CharT, typename T>
inline std::string to_string(const co::basic_variable_map<CharT> &vm,
  const co::value<T> &)
{
  std::stringstream out;

  for(auto && pair : vm) {
    out << pair.first << " -> ";
    if(co::is_empty(pair.second))
      out << "[empty]\n";
    else
      out << co::any_cast<T>(pair.second) << "\n";
  }

  return out.str();
}


template<typename CharT>
bool check_exclusive(const co::basic_option_description<CharT> &desc,
  bool(*fn)(const co::basic_option_description<CharT> &))
{
  typedef bool(*fn_t)(const co::basic_option_description<CharT> &);
  typedef std::pair<std::basic_string<CharT>,fn_t> pair_type;

  static const std::vector<pair_type> cases{
    {_LIT("is_mapped_isolated_option"),
      is_mapped_isolated_option<CharT>},
    {_LIT("is_hidden_mapped_isolated_option"),
      is_hidden_mapped_isolated_option<CharT>},
    {_LIT("is_raw_isolated_option"),
      is_raw_isolated_option<CharT>},
    {_LIT("is_hidden_raw_isolated_option"),
      is_hidden_raw_isolated_option<CharT>},
    {_LIT("is_mapped_reqired_option"),
      is_mapped_reqired_option<CharT>},
    {_LIT("is_hidden_mapped_reqired_option"),
      is_hidden_mapped_reqired_option<CharT>},
    {_LIT("is_raw_reqired_option"),
      is_raw_reqired_option<CharT>},
    {_LIT("is_raw_mapped_reqired_option"),
      is_raw_mapped_reqired_option<CharT>},
    {_LIT("is_mapped_optional_option"),
      is_mapped_optional_option<CharT>},
    {_LIT("is_hidden_mapped_optional_option"),
      is_hidden_mapped_optional_option<CharT>},
    {_LIT("is_raw_optional_option"),
      is_raw_optional_option<CharT>},
    {_LIT("is_raw_mapped_optional_option"),
      is_raw_mapped_optional_option<CharT>},
    {_LIT("is_interpret_operand"),
      is_interpret_operand<CharT>},
    {_LIT("is_hidden_interpret_operand"),
      is_hidden_interpret_operand<CharT>},
    {_LIT("is_keyed_interpret_operand"),
      is_keyed_interpret_operand<CharT>},
    {_LIT("is_hidden_keyed_interpret_operand"),
      is_hidden_keyed_interpret_operand<CharT>},
    {_LIT("is_empty_operand"),
      is_empty_operand<CharT>},
    {_LIT("is_hidden_empty_operand"),
      is_hidden_empty_operand<CharT>},
    {_LIT("is_empty_keyed_operand"),
      is_empty_keyed_operand<CharT>},
    {_LIT("is_hidden_empty_keyed_operand"),
      is_hidden_empty_keyed_operand<CharT>}
  };

  std::vector<pair_type> exclusive = cases;
  typename std::vector<pair_type>::iterator loc =
    std::remove_if(exclusive.begin(),exclusive.end(),
      [&](const pair_type &val) {
        return val.second == fn;
      }
    );

  assert(loc != exclusive.end());

  exclusive.erase(loc,exclusive.end());

#if 1
  if(!fn(desc)) {
    std::cerr
      << "Given function returned false\n"
      << to_string(desc) << "\n";
    return false;
  }

  for(std::size_t i=0; i<exclusive.size(); ++i) {
    if(exclusive[i].second(desc)) {
      std::cerr
        << "check_exclusive is true for : " << exclusive[i].first
        << "\n" << to_string(desc) << "\n";
      return false;
    }
  }
  return true;
#endif

  return fn(desc) && std::none_of(exclusive.begin(),exclusive.end(),
    [&](const pair_type &val) {return val.second(desc);});
}

// should try and move away from this to per-value version \c vm_check
template<typename T, typename VariableMap>
bool contents_equal(const VariableMap &lhs, const VariableMap &rhs)
{
  typedef typename VariableMap::key_type::value_type char_type;
  typename VariableMap::const_iterator lcur = lhs.begin();
  typename VariableMap::const_iterator rcur = rhs.begin();

  while(lcur != lhs.end() && rcur != rhs.end()) {
    if(lcur->first != rcur->first) {
      std_stream_select<char_type>::cerr
        << _LIT("lhs vm key '") << lcur->first
        << _LIT("' != rhs vm key '") << rcur->first
        << _LIT("'\n");
      return false;
    }

    if(!(co::is_empty(lcur->second) && co::is_empty(rcur->second))) {
      if(co::is_empty(lcur->second) && !co::is_empty(rcur->second)) {
        std_stream_select<char_type>::cerr
          << _LIT("lhs vm value for key '") << lcur->first
          << _LIT("' is empty but rhs vm value for key '")
          << rcur->first << _LIT("' is not\n");
        return false;
      }

      if(!co::is_empty(lcur->second) && co::is_empty(rcur->second)) {
        std_stream_select<char_type>::cerr
          << _LIT("lhs vm value for key '") << lcur->first
          << _LIT("' is not empty but rhs vm value for key '")
          << rcur->first << _LIT("' is\n");
        return false;
      }

      try {
        if(co::any_cast<T>(lcur->second) != co::any_cast<T>(rcur->second))
          return false;
      }
      catch(const co::bad_any_cast &ex) {
        std_stream_select<char_type>::cerr
          << _LIT("vm values are not given type T: ") << ex.what()
          << _LIT("\n");
        throw;
      }
    }

    ++lcur;
    ++rcur;
  }

  return lcur == lhs.end() && rcur == rhs.end();
}


template<typename CharT>
std::function<bool(
  const typename co::basic_variable_map<CharT>::value_type &)>
check_empty(const std::basic_string<CharT> &key)
{
  return [=](const typename co::basic_variable_map<CharT>::value_type &val) {
    if(key != val.first) {
      std_stream_select<CharT>::cerr
        << _LIT("vm key '") << val.first
        << _LIT("' does not equal required key '" << key << _LIT("\n"));
      return false;
    }
    if(!co::is_empty(val.second)) {
      std_stream_select<CharT>::cerr
        << _LIT("vm value for key '") << val.first
        << _LIT("' should be empty\n");
      return false;
    }

    return true;
  };
}

template<typename CharT, typename T>
std::function<bool(
  const typename co::basic_variable_map<CharT>::value_type &)>
check_empty(const CharT *key)
{
  return check_empty(std::basic_string<CharT>(key));
}

#if 0
template<typename T>
struct approximatelyEqual {
  approximatelyEqual(void) :_epsilon() {}

  bool operator()(T a, T b)
  {
    return std::fabs(a-b) <=
      ((std::fabs(a) < std::fabs(b)?std::fabs(b):std::fabs(a)) *
        std::numeric_limits<T>::epsilon());
  }

  T _epsilon;
};
#endif

template<typename T>
struct essentiallyEqual {
  bool operator()(const T &lhs, const T &rhs) const
  {
    return
      std::fabs(lhs-rhs) <=
        ((std::fabs(lhs) > std::fabs(rhs)?std::fabs(rhs):std::fabs(lhs)) *
          .0001); // .001%
  }
};

template<typename CharT, typename T, typename Eq=std::equal_to<T> >
std::function<bool(
  const typename co::basic_variable_map<CharT>::value_type &)>
check_value(const std::basic_string<CharT> &key, const T &value,
  const Eq &eq = Eq())
{
  return [=](const typename co::basic_variable_map<CharT>::value_type &val) {
    if(co::is_empty(val.second)) {
      std_stream_select<CharT>::cerr
        << _LIT("vm value for key '") << val.first
        << _LIT("' is empty shouldn't be\n");
      return false;
    }
    else if(key != val.first) {
      std_stream_select<CharT>::cerr
        << _LIT("vm key '") << val.first
        << _LIT("' does not equal required key '" << key << _LIT("\n"));
      return false;
    }

    try {
//
//       std_stream_select<CharT>::cerr
//         << co::detail::asUTF8(_LIT("checking: '"))
//         << val.second.type().name() << co::detail::asUTF8(_LIT("' ("))
//         << typeid(T).name()
//         << co::detail::asUTF8(_LIT(")\n"))
//         << std::setprecision(std::numeric_limits<T>::digits10 + 1)
//         << co::any_cast<T>(val.second)
//         << co::detail::asUTF8(_LIT(" and "))
//         << value << co::detail::asUTF8(_LIT("\n"));

      return eq(co::any_cast<T>(val.second),value);
    }
    catch(const co::bad_any_cast &ex) {
      std_stream_select<CharT>::cerr
        << _LIT("vm values are not given type T: ") << ex.what()
        << _LIT("\n");
      throw;
    }
  };
}

template<typename CharT, typename T, typename Eq=std::equal_to<T> >
std::function<bool(
  const typename co::basic_variable_map<CharT>::value_type &)>
check_value(const CharT *key, const T &value, const Eq &eq = Eq())
{
  return check_value(std::basic_string<CharT>(key),value,eq);
}

template<typename VariableMap>
bool vm_check(const VariableMap &vm,
  const std::vector<
    std::function<bool(const typename VariableMap::value_type &)> > &pred_vec)
{
  typedef typename VariableMap::key_type::value_type char_type;
  typename VariableMap::const_iterator lcur = vm.begin();

  if(vm.size() != pred_vec.size()) {
    std_stream_select<char_type>::cerr
      << _LIT("lhs vm is not the same size as the predicate list\n");
    return false;
  }

  for(auto &pred : pred_vec) {
    if(!pred(*lcur++))
      return false;
  }

  return true;
}


}

#endif
