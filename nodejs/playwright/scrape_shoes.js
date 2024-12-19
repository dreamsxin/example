import asyncio
import json
from contextlib import suppress
from urllib.parse import parse_qs, urlparse
from playwright.async_api import Page, Playwright, Response, async_playwright

async def scroll_to_bottom(page: Page) -> None:
    # Get the initial scroll height of the page
    last_height = await page.evaluate("document.body.scrollHeight")
    iteration = 1

    while True:
        print(f"Scrolling page {iteration}...")

        # Scroll to the bottom of the page
        await page.evaluate("window.scrollTo(0, document.body.scrollHeight);")

        # Wait for the page to load additional content
        await asyncio.sleep(1)

        # Get the new scroll height and compare it with the last height
        new_height = await page.evaluate("document.body.scrollHeight")

        if new_height == last_height:
            break  # Exit the loop if the bottom of the page is reached
        last_height = new_height
        iteration += 1

async def block_resources(page: Page) -> None:
    async def intercept_route(route, request):
        if request.resource_type in ["image", "stylesheet", "font", "xhr"]:
            await route.abort()
        else:
            await route.continue_()

    await page.route("**/*", intercept_route)

async def extract_product_data(response: Response, extracted_products: list) -> None:
    # Parse the URL and extract query parameters
    parsed_url = urlparse(response.url)
    query_params = parse_qs(parsed_url.query)

    # Check if the URL contains 'queryid=products'
    if "queryid" in query_params and query_params["queryid"][0] == "products":
        # Get the JSON response
        data = await response.json()

        # Use suppress to handle potential KeyError exceptions
        with suppress(KeyError):
            # Iterate through the products and extract details
            for product in data["data"]["products"]["products"]:
                product_details = {
                    "colorDescription": product["colorDescription"],
                    "currency": product["price"]["currency"],
                    "currentPrice": product["price"]["currentPrice"],
                    "fullPrice": product["price"]["fullPrice"],
                    "inStock": product["inStock"],
                    "title": product["title"],
                    "subtitle": product["subtitle"],
                    "url": product["url"].replace(
                        "{countryLang}", "https://www.nike.com/en"
                    ),
                }

                extracted_products.append(product_details)

async def scrape_shoes(playwright: Playwright, target_url: str) -> None:
    browser = await playwright.chromium.launch(headless=False)
    page = await browser.new_page(viewport={"width": 1600, "height": 900})

    # Block unnecessary resources
    await block_resources(page)

    extracted_products = []

    # Set up a response event handler to extract the product data
    page.on(
        "response", lambda response: extract_product_data(response, extracted_products)
    )

    # Navigate to the target URL
    await page.goto(target_url)
    await asyncio.sleep(2)

    # Scroll to the bottom of the page to load all products
    await scroll_to_bottom(page)

    # Save the extracted data to a JSON file
    with open("extracted_products.json", "w") as file:
        json.dump(extracted_products, file, indent=4)
    await browser.close()

async def main() -> None:
    async with async_playwright() as playwright:
        await scrape_shoes(
            playwright=playwright,
            target_url="https://www.nike.com/w/mens-lifestyle-shoes-13jrmznik1zy7ok",
        )

if __name__ == "__main__":
    asyncio.run(main())
