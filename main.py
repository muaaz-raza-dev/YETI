from fastapi import FastAPI

app = FastAPI(title="YETI API")
@app.get("/")
async def root():
    return {"message": "YETI API is running"}