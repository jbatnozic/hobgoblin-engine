@spritesheet theme 
{
%%fp:spritesheet_begin

	%%fp:src: -f default-spritesheet.tga -w 1024 -h 1024

	%%fp:sprite: -n frame    -f ./frame.png
	%%fp:sprite: -n titlebar -f ./titlebar.png

%%fp:spritesheet_end
}

body
{
	font-family: LatoLatin;
	font-weight: bold;
	font-style: normal;
	font-size: 20dp;
	color: rgb(0, 0, 0);
}

body.window {
	decorator: image(frame);
}

div {
    display: block;
}

handle {
	display: block;
}

div#title-bar {
    position: absolute;
    top: -45dp;
}

div#title-bar span {
	display: inline-block;
	width: 100%;

	font-size: 22px;
	font-weight: bold;

	box-sizing: border-box;
    padding-left: 20dp;
	padding-right: 20dp;
	padding-top: 15dp;
	padding-bottom: 15dp;

	font-effect: shadow(2px 2px rgba(2, 2, 2, 0.425));
	color: white;

    decorator: image(titlebar);
}

.window-contents {
    padding-left: 20dp;
	padding-right: 20dp;
	padding-top: 20dp;
	padding-bottom: 20dp;
}

table {
	box-sizing: border-box;
	display: table;
}
tr {
	box-sizing: border-box;
	display: table-row;
}
tr:nth-child(odd) {
	background-color: #a1a1a1;
}
td {
	box-sizing: border-box;
	display: table-cell;
}
col {
	box-sizing: border-box;
	display: table-column;
}
colgroup {
	display: table-column-group;
}
thead, tbody, tfoot {
	display: table-row-group;
}

.centred-column {
	text-align: center;
}
.button {
	text-align: center;
}
.button:hover {
	text-align: center;
	background-color: #70ceeb;
}
.button:active {
	text-align: center;
	background-color: #354449;
}