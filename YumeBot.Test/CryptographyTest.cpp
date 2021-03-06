#include <Cryptography.h>
#include <catch2/catch.hpp>
#include <cstring>
#include <iterator>

using namespace YumeBot;
using namespace Cafe;
using namespace Encoding;
using namespace StringLiterals;

TEST_CASE("Cryptography", "[Utility][Cryptography]")
{
	using namespace Cryptography;

	SECTION("Tea")
	{
		using namespace Tea;

		constexpr const char text[] = "123456789123456789";
		constexpr const char key[] = "00000000000000000000000000000000";

		char result[CalculateOutputSize(std::size(text))]{};
		const auto formattedKey = FormatKey(gsl::as_bytes(gsl::make_span(key)));

		// 0x30303030 == { '0', '0', '0', '0' }
		REQUIRE(std::all_of(
		    std::cbegin(formattedKey), std::cend(formattedKey),
		    [](std::uint32_t value) constexpr { return value == 0x30303030; }));

		const auto resultSize = Encrypt(gsl::as_bytes(gsl::make_span(text)),
		                                gsl::as_writeable_bytes(gsl::make_span(result)), formattedKey);
		char decryptResult[std::size(result)]{};
		const auto decryptResultSize =
		    Decrypt(gsl::as_bytes(gsl::make_span(result)),
		            gsl::as_writeable_bytes(gsl::make_span(decryptResult)), formattedKey);

		REQUIRE(resultSize == CalculateOutputSize(std::size(text)));
		REQUIRE(decryptResultSize == std::size(text));
		REQUIRE(std::memcmp(decryptResult, text, std::size(text)) == 0);
	}

	SECTION("Md5")
	{
		using namespace Md5;

		constexpr const char test[] = "test";

		std::byte result[16];
		Calculate(gsl::as_bytes(gsl::make_span(test)).subspan(0, std::size(test) - 1), result);

		constexpr const std::uint8_t expectedResult[] =
		    "\x09\x8f\x6b\xcd\x46\x21\xd3\x73\xca\xde\x4e\x83\x26\x27\xb4\xf6";
		constexpr const auto expectedResultStr = u8"098f6bcd4621d373cade4e832627b4f6"_sv;
		REQUIRE(std::memcmp(result, expectedResult, std::size(result)) == 0);
		Md5ToHexString(result, [&](StringView<CodePage::Utf8> const& str) {
			REQUIRE(str == expectedResultStr.Trim());
		});
	}
}
