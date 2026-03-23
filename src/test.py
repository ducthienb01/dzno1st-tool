from selenium import webdriver

def open_link():
    driver = webdriver.Chrome()
    
    driver.get("https:\/\/www.facebook.com\/808547812293958")
    
    input("Press Enter to close...")  # giữ cửa sổ lại để bạn xem
    
    driver.quit()

if __name__ == "__main__":
    open_link()