import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure Flask
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Check cache
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session access to fiel system 
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Access SQLite database
db = SQL("sqlite:///finance.db")

# Check api Key
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # Access user information
    user = db.execute("SELECT * FROM users WHERE id = :id",
                      id=session["user_id"])[0]["cash"]
    user = round(user, 2)
    transactions = db.execute(
        "SELECT * FROM purchases WHERE id = :id", id=session["user_id"])

    # Save in list
    stocks = []

    # Get all data 
    for trans in transactions:
        symbol = trans["stock"]
        shares = trans["shares"]
        stock_data = lookup(symbol)

        temp_dict = {
            "symbol": symbol,
            "name": stock_data["name"],
            "shares": shares,
            "price": round(stock_data["price"], 2),
            "total": round(stock_data["price"] * shares, 2)
        }

        stocks.append(temp_dict)

    # get users networth
    total_money = round(user + sum([stock["total"] for stock in stocks]), 2)

    return render_template("index.html", stocks=stocks, money=total_money, user=user)


@app.route("/change_password", methods=["GET", "POST"])
@login_required
def change():
    if request.method == "GET":
        return render_template("password.html")
    else:
        password = request.form.get("password")

        if password == "":
            return apology("The password is too short")

        pass_hash = generate_password_hash(password)

        db.execute("UPDATE users SET hash = :pw WHERE id = :id",
                   pw=pass_hash, id=session["user_id"])

        return redirect("/")


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "GET":
        return render_template("buy.html")
    else:
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        if not shares.isdigit() or float(shares) < 1:
            return apology("The quantity must be an integer and at least 1")

        stock = lookup(symbol)

        # stock does not exist
        if stock is None or symbol == "":
            return apology("Stock absent")

        # look if user has enough money
        total_price = float(shares) * stock["price"]
        available_cash = db.execute("SELECT cash FROM users WHERE id = :id",
                               id=session["user_id"])[0]["cash"]
        if total_price > available_cash:
            return apology("You do not have enough money to complete purchase. Top up yor credits")

        # complete purchase
        residualMoney = available_cash - total_price
        db.execute(
            f"UPDATE users SET cash = {residualMoney} WHERE id = :id", id=session["user_id"])

        # look if stock already exists
        stocks = db.execute(
            "SELECT * FROM purchases WHERE stock = :symbol AND id = :id", symbol=symbol, id=session["user_id"])

        if len(stocks) == 0:
            db.execute("INSERT INTO purchases (id, price, shares, stock) VALUES (:id, :price, :shares, :stock)",
                       id=session["user_id"], price=stock["price"], shares=shares, stock=stock["symbol"])
        else:
            db.execute(
                f"UPDATE purchases SET shares = {stocks[0]['shares'] + shares}, price = {stock['price']} WHERE id = :id and stock = :stock", id=session["user_id"], stock=symbol)

        # add to history
        db.execute("INSERT INTO history (symbol, price, dt, id, shares, buy) VALUES (:symbol, :price, DATETIME('now'), :id, :shares, :buy)",
                   symbol=symbol, price=stock["price"], id=session["user_id"], shares=shares, buy=1)

    return redirect("/")


@app.route("/check", methods=["GET"])
def check():
    """Return true if username available, else false, in JSON format"""

    username = request.args.get("username")

    if len(username) < 1:
        return jsonify(False)

    check_username = db.execute(
        "SELECT username FROM users WHERE username = :un", un=username)

    if len(check_username) == 0:
        return jsonify(True)
    else:
        return jsonify(False)


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # get user history
    hist = db.execute("SELECT * FROM history WHERE id = :id ORDER BY dt DESC",
                      id=session["user_id"])

    # create list 
    transactions = []

    for trans in hist:
        temp_dict = {
            "date": trans["dt"],
            "symbol": trans["symbol"],
            "transaction": "BOUGHT" if trans["buy"] == 1 else "SOLD",
            "shares": trans["shares"],
            "price": trans["price"],
            "total": trans["price"] * trans["shares"]
        }

        transactions.append(temp_dict)

    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # Check HTTP method
    if request.method == "POST":

        # Check username 
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Check password
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Check username and password match
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Store user session
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # handel http methond != POST
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    if request.method == "GET":
        return render_template("quote.html")
    else:
        try:
            stock = lookup(request.form.get("symbol"))
        except:
            return apology("There is no stock with that name")

        if stock is None:
            return apology("There is no stock with that name")

        return render_template("quoted.html", stock=stock)

    return apology("TODO")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    if request.method == "POST":

        # get the args
        user = request.form.get("username")
        password = request.form.get("password")
        conf = request.form.get("confirmation")

        rows = db.execute(
            "SELECT * FROM users WHERE username = :user", user=user)

        # check for username and input
        if user == "":
            return apology("You need to provide a username")
        elif len(rows) == 1:
            return apology("Username already taken")

        # check for username
        if password == "":
            return apology("You need to provide a password")
        elif password != conf:
            return apology("Passwords need to match")

        # hash the password
        crypted_password = generate_password_hash(password)

        # add to database
        db.execute("INSERT INTO users (username, hash) VALUES (:user, :password)",
                   user=user, password=crypted_password)

        return redirect("/login")
    else:
        return render_template("register.html")

    return apology("TODO")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "GET":

        all_stocks = db.execute(
            "SELECT stock FROM purchases WHERE id = :id", id=session["user_id"])

        return render_template("sell.html", stocks=all_stocks)
    else:

        # get all values
        symbol = request.form.get("symbol")
        shares = int(request.form.get("shares"))
        user_shares = db.execute(
            "SELECT shares FROM purchases WHERE id = :id AND stock = :symbol", id=session["user_id"], symbol=symbol)[0]["shares"]
        available_cash = db.execute(
            "SELECT cash FROM users WHERE id = :id", id=session["user_id"])[0]["cash"]
        current_price = lookup(symbol)["price"]

        # error check
        if symbol == "" or shares == "":
            return apology("You need to enter a stock and share amount")

        if shares <= 0:
            return apology("You need to provide a positive integer")

        if user_shares < shares:
            return apology("You do not have that many shares to sell")

        # update database

        db.execute(f"UPDATE purchases SET shares = {user_shares - shares} WHERE id = :id AND stock = :symbol",
                   id=session["user_id"], symbol=symbol)

        db.execute(f"UPDATE users SET cash = {available_cash + (current_price * shares)} WHERE id = :id",
                   id=session["user_id"])

        db.execute("INSERT INTO history (symbol, price, dt, id, shares, buy) VALUES (:symbol, :price, DATETIME('now'), :id, :shares, :buy)",
                   symbol=symbol, price=current_price, id=session["user_id"], shares=shares, buy=0)

        return redirect("/")

    return apology("TODO")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
