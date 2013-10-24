function showAbout() {
	$("div.article").find("div.sectionAbout:hidden").slideDown("slow"); 
	$("div.article").find("div.sectionConnections:visible").slideUp("slow"); 
	$("div.article").find("div.sectionUsers:visible").slideUp("slow"); 
	$("div.article").find("div.sectionSettings:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRestrictions:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRelated:visible").slideUp("fast"); 
}
function showConnections() {
	$("div.article").find("div.sectionAbout:visible").slideUp("slow"); 
	$("div.article").find("div.sectionConnections:hidden").slideDown("slow"); 
	$("div.article").find("div.sectionUsers:visible").slideUp("slow"); 
	$("div.article").find("div.sectionSettings:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRestrictions:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRelated:visible").slideUp("fast"); 
}
function showUsers() {
	$("div.article").find("div.sectionAbout:visible").slideUp("slow"); 
	$("div.article").find("div.sectionConnections:visible").slideUp("slow"); 
	$("div.article").find("div.sectionUsers:hidden").slideDown("slow"); 
	$("div.article").find("div.sectionSettings:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRestrictions:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRelated:visible").slideUp("fast"); 
}
function showSettings() {
	$("div.article").find("div.sectionAbout:visible").slideUp("slow"); 
	$("div.article").find("div.sectionConnections:visible").slideUp("slow"); 
	$("div.article").find("div.sectionUsers:visible").slideUp("slow"); 
	$("div.article").find("div.sectionSettings:hidden").slideDown("slow"); 
	$("div.article").find("div.sectionRestrictions:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRelated:visible").slideUp("fast"); 
}
function showRestrictions() {
	$("div.article").find("div.sectionAbout:visible").slideUp("slow"); 
	$("div.article").find("div.sectionConnections:visible").slideUp("slow"); 
	$("div.article").find("div.sectionUsers:visible").slideUp("slow"); 
	$("div.article").find("div.sectionSettings:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRestrictions:hidden").slideDown("slow"); 
	$("div.article").find("div.sectionRelated:visible").slideUp("fast"); 
}								
function showRelated() {
	$("div.article").find("div.sectionAbout:visible").slideUp("slow"); 
	$("div.article").find("div.sectionConnections:visible").slideUp("slow"); 
	$("div.article").find("div.sectionUsers:visible").slideUp("slow"); 
	$("div.article").find("div.sectionSettings:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRestrictions:visible").slideUp("slow"); 
	$("div.article").find("div.sectionRelated:hidden").slideDown("fast"); 
}		