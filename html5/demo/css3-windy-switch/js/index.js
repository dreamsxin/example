	$("#switch").on('click', function () {
		if ($("body").hasClass("slow-wind")) {
			$("body").removeClass("slow-wind");
			$("#switch").removeClass("switched");
		}
		else {
			$("body").addClass("slow-wind");
			$("#switch").addClass("switched");

		}
	});